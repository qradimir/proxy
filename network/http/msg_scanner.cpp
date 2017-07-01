//
// Created by radimir on 12/05/17.
//

#include "msg_scanner.h"
#include "read_util.h"
#include "parse_error.h"

namespace network { namespace http
{

    class sized_msg_scanner : public msg_scanner
    {
    private:
        size_t remaining_size;
    public:
        sized_msg_scanner(util::composed_buffer<char> *read_buffer, util::composed_buffer<char> *write_buffer,
                          size_t expected_size);

        bool try_advance() override;
    };

    class chunked_msg_scanner : public msg_scanner
    {
    private:
        size_t remaining_chunk_size;
        bool reads_chunk_size;
    public:
        chunked_msg_scanner(util::composed_buffer<char> *read_buffer, util::composed_buffer<char> *write_buffer);

        bool try_advance() override;
    };

    std::unique_ptr<msg_scanner>
    scanner_for(request const &req, util::composed_buffer<char> *read_buffer, util::composed_buffer<char> *write_buffer)
    {
        header_map const &headers = req.get_headers();
        header_map::const_iterator transfer_encoding_it = req.get_headers().find("Transfer-Encoding");
        if (transfer_encoding_it != headers.end()) {
            std::string const &transfer_encoding = transfer_encoding_it->second;
            if (transfer_encoding == "chunked") {
                return std::make_unique<chunked_msg_scanner>(read_buffer, write_buffer);
            }
        }
        header_map::const_iterator content_length_it = req.get_headers().find("Content-Length");
        if (content_length_it != headers.end()) {
            size_t content_length = std::stoul(content_length_it->second);
            return std::make_unique<sized_msg_scanner>(read_buffer, write_buffer, content_length);
        }
        return {};
    }

    msg_scanner::msg_scanner(util::composed_buffer<char> *read_buffer, util::composed_buffer<char> *write_buffer)
            : read_buffer(read_buffer), write_buffer(write_buffer)
    {
    }

    sized_msg_scanner::sized_msg_scanner(util::composed_buffer<char> *read_buffer,
                                         util::composed_buffer<char> *write_buffer,
                                         size_t expected_size
    )
            : msg_scanner(read_buffer, write_buffer), remaining_size(expected_size)
    {
    }

    bool sized_msg_scanner::try_advance()
    {
        remaining_size -= read_buffer->forward(write_buffer, remaining_size);

        return remaining_size == 0;
    }

    chunked_msg_scanner::chunked_msg_scanner(util::composed_buffer<char> *read_buffer,
                                             util::composed_buffer<char> *write_buffer)
            : msg_scanner(read_buffer, write_buffer), remaining_chunk_size(0), reads_chunk_size(true)
    {
    }

    void write_(util::composed_buffer<char> *buf, std::string const &str)
    {
        std::vector<char> vec{str.begin(), str.end()};
        buf->write(vec);
    }

    bool chunked_msg_scanner::try_advance()
    {
        while (true) {
            if (remaining_chunk_size == 0 && !reads_chunk_size)
                return true;

            if (reads_chunk_size) {
                std::unique_ptr<std::string> chunk_size_ref = scan_word_until_crlf(read_buffer);
                if (chunk_size_ref) {
                    remaining_chunk_size = std::stoul(*chunk_size_ref, 0, 16);
                    reads_chunk_size = false;
                    write_(write_buffer, *chunk_size_ref);
                    write_(write_buffer, CRLF);
                }
            } else {
                reads_chunk_size -= read_buffer->forward(write_buffer, remaining_chunk_size);
                if (remaining_chunk_size == 0) {
                    if (matches(read_buffer->begin(), read_buffer->end(), CRLF)) {
                        read_buffer->read(read_buffer->begin() += CRLF.size());
                        write_(write_buffer, CRLF);
                        reads_chunk_size = true;
                    } else {
                        throw parse_error{"Chunk size mismatched"};
                    }
                }
            }
        }

    }
}}

