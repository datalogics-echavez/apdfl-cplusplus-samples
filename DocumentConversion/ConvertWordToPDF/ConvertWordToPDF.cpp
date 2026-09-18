// Copyright (c) 2025, Datalogics, Inc. All rights reserved.
//
// ConvertWordToPDF
//
// Demonstrates the Datalogics Office-to-PDF SDK's plain-C interface
// (office_to_pdf/converter_c.h): convert a Microsoft Word .docx document to
// PDF, both from a file on disk and from a document held in memory.
//
// The SDK initializes the Adobe PDF Library itself for the duration of each
// conversion, so -- unlike the other samples here -- this one creates no
// APDFLib of its own and includes only the C ABI header.
//
// Usage:
//   ConvertWordToPDF [input.docx] [output.pdf]
//
// With no arguments it converts a bundled sample document to
// ConvertWordToPDF-out.pdf in the current directory.

#include <office_to_pdf/converter_c.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#define DEF_INPUT "../../../../Resources/Sample_Input/DOCXLink.docx"
#define DEF_OUTPUT "ConvertWordToPDF-out.pdf"

namespace {

// Translate the C status enum to text. Reporting the enum explicitly rather
// than its raw integer is the same discipline the C ABI itself follows across
// the boundary, and keeps the sample legible.
const char *StatusText(o2pdf_status status) {
    switch (status) {
    case O2PDF_STATUS_SUCCESS:
        return "success";
    case O2PDF_STATUS_INPUT_NOT_FOUND:
        return "input not found";
    case O2PDF_STATUS_INVALID_INPUT:
        return "invalid input (not a .docx)";
    case O2PDF_STATUS_INPUT_PROTECTED:
        return "input is password-protected";
    case O2PDF_STATUS_DESTINATION_NOT_WRITABLE:
        return "destination not writable";
    case O2PDF_STATUS_CONVERSION_ERROR:
        return "conversion error";
    case O2PDF_STATUS_FORCE_INT32:
        break;
    }
    return "unknown status";
}

// Print any per-asset diagnostics the conversion reported. A newer library may
// return a diagnostic kind this build's header predates; converter_c.h's
// forward-compatibility contract is to treat an unknown (higher-numbered) kind
// as "other" and read the accompanying asset/message, which is what this does
// by printing the raw kind value next to its text.
void ReportDiagnostics(const o2pdf_result *result) {
    const size_t count = o2pdf_result_diagnostic_count(result);
    if (count == 0)
        return;
    std::cout << "  " << count << " diagnostic(s):" << std::endl;
    for (size_t i = 0; i < count; ++i) {
        std::cout << "    [kind "
                  << static_cast<int>(o2pdf_result_diagnostic_kind(result, i))
                  << "] " << o2pdf_result_diagnostic_asset(result, i) << ": "
                  << o2pdf_result_diagnostic_message(result, i) << std::endl;
    }
}

} // namespace

int main(int argc, char **argv) {
    const std::string input = (argc > 1) ? argv[1] : DEF_INPUT;
    const std::string output = (argc > 2) ? argv[2] : DEF_OUTPUT;

    std::cout << "ConvertWordToPDF (office-to-pdf-sdk " << o2pdf_version_string()
              << ")" << std::endl;

    // Deterministic conversion options: pin the produced PDF's creation and
    // modification dates so repeated runs are byte-reproducible, and omit Word
    // comments. Passing NULL instead of &options would use the SDK defaults
    // (system clock, comments omitted).
    o2pdf_options options;
    options.has_conversion_time = 1;
    options.conversion_time.year = 2025;
    options.conversion_time.month = 1;
    options.conversion_time.day = 1;
    options.conversion_time.hour = 0;
    options.conversion_time.minute = 0;
    options.conversion_time.second = 0;
    options.comments = O2PDF_COMMENTS_OMIT;

    int exit_code = 0;

    //=========================================================================
    // 1) File-to-file conversion (o2pdf_convert_file).
    //=========================================================================
    std::cout << "\nConverting file: " << input << " -> " << output << std::endl;
    o2pdf_result *file_result =
        o2pdf_convert_file(input.c_str(), output.c_str(), &options);
    const o2pdf_status file_status = o2pdf_result_status(file_result);
    std::cout << "  status:  " << StatusText(file_status) << std::endl;
    std::cout << "  message: " << o2pdf_result_message(file_result) << std::endl;
    ReportDiagnostics(file_result);
    if (file_status != O2PDF_STATUS_SUCCESS)
        exit_code = 1;
    o2pdf_result_free(file_result);

    //=========================================================================
    // 2) In-memory conversion (o2pdf_convert_buffer / o2pdf_result_output):
    //    read the .docx into a buffer, convert, and write the returned PDF
    //    bytes out. This exercises the second entry point and the output
    //    accessor.
    //=========================================================================
    std::ifstream in(input, std::ios::binary);
    if (in) {
        const std::vector<unsigned char> bytes(
            (std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());
        in.close();

        std::cout << "\nConverting " << bytes.size() << " bytes in memory"
                  << std::endl;
        o2pdf_result *buf_result =
            o2pdf_convert_buffer(bytes.data(), bytes.size(), &options);
        const o2pdf_status buf_status = o2pdf_result_status(buf_result);
        std::cout << "  status:  " << StatusText(buf_status) << std::endl;
        std::cout << "  message: " << o2pdf_result_message(buf_result)
                  << std::endl;
        ReportDiagnostics(buf_result);

        const unsigned char *pdf_bytes = nullptr;
        size_t pdf_len = 0;
        if (o2pdf_result_output(buf_result, &pdf_bytes, &pdf_len)) {
            const std::string buffer_output = "ConvertWordToPDF-buffer-out.pdf";
            std::ofstream out(buffer_output, std::ios::binary);
            out.write(reinterpret_cast<const char *>(pdf_bytes),
                      static_cast<std::streamsize>(pdf_len));
            std::cout << "  wrote " << pdf_len << " bytes to " << buffer_output
                      << std::endl;
        } else if (buf_status != O2PDF_STATUS_SUCCESS) {
            exit_code = 1;
        }
        o2pdf_result_free(buf_result);
    } else {
        std::cout << "\nSkipping in-memory conversion: could not read " << input
                  << std::endl;
    }

    return exit_code;
}
