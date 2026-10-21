#include "luac/source.hpp"
#include "lasm/output_stream.hpp"

void Luac::Source::print(Lasm::OutputStream* stream) {
    stream->reserve(stream->m_indent + 8);
    stream->write('\t', stream->m_indent);
    stream->write(".source ");
    String::print(stream);
    stream->reserve(1);
    stream->write("\n");
}