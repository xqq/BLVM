#ifndef _BLVM_BITCODE_PARSING_EXCEPTION_HPP
#define _BLVM_BITCODE_PARSING_EXCEPTION_HPP

#include <stdexcept>

namespace blvm {
namespace bitcode {

    enum ReaderError : int {
        kEof = 1,
        kDataNotEnough = 2,
        kDataError = 3,
        kScopeMismatch = 4
    };

    class ReaderException : public std::exception {
    public:
        explicit ReaderException(int error_code) : code_(error_code) {}
        virtual ~ReaderException() = default;
        int code() {
            return code_;
        }
    private:
        int code_;
    };


    enum ParserError : int {
        kNull = 0
    };

    class ParserException : public std::exception {
    public:
        explicit ParserException(int error_code) : code_(error_code) {}
        virtual ~ParserException() = default;
        int code() {
            return code_;
        }
    private:
        int code_;
    };

}
}

#endif // _BLVM_BITCODE_PARSING_EXCEPTION_HPP
