#ifndef _BLVM_BITCODE_PARSING_EXCEPTION_HPP
#define _BLVM_BITCODE_PARSING_EXCEPTION_HPP

#include <stdexcept>

namespace blvm {
namespace bitcode {

    // TODO: pass message, __FILE__, __LINE__, __FUNC__, etc. params to exception object,
    // TODO: In order to to report a detailed error to upper-layer.

    enum class ReaderError : int {
        kEof = 1,
        kDataNotEnough = 2,
        kDataError = 3,
        kScopeMismatch = 4
    };

    class ReaderException : public std::exception {
    public:
        explicit ReaderException(ReaderError error_code) : code_(error_code) {}
        virtual ~ReaderException() = default;
        int code() {
            return static_cast<int>(code_);
        }
    private:
        ReaderError code_;
    };


    enum class ParserError : int {
        kDataError = 1,
        kNotSupproted = 2,
    };

    class ParserException : public std::exception {
    public:
        explicit ParserException(ParserError error_code) : code_(error_code) {}
        virtual ~ParserException() = default;
        int code() {
            return static_cast<int>(code_);
        }
    private:
        ParserError code_;
    };

}
}

#endif // _BLVM_BITCODE_PARSING_EXCEPTION_HPP
