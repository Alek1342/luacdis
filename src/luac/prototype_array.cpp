
#include "log.hpp"
#include "luac/array.hpp"
#include "luac/prototype.hpp"
#include "stack.hpp"
#include <cstdlib>

template <>
void Luac::Array<Luac::Prototype>::init(i32 length) {
    m_length = length;
    if (length > 0) {
        Prototype* items = static_cast<Prototype*>(calloc(length, sizeof(Prototype)));
        if (!items) {
            LOGF("Luac::Array<Prototype>.init malloc failed");
        }

        m_items = items;
    }
}

template <>
Luac::Array<Luac::Prototype>::~Array() {
    if (m_items) {
        Stack<Prototype*> ps;
        Stack<size_t> cnts;
        size_t cnt = m_length;
        Prototype* p = m_items;
        while (true) {
            if (p->m_functions.m_length > 0) {
                ps.push(p);
                cnts.push(cnt);
                cnt = p->m_functions.m_length;
                p = p->m_functions.m_items;
            } else {
                p->m_code.~Array();
                p->m_constants.~Array();
                p->m_upvalues.~Array();
                p->m_source.~Source();
                p->m_lineinfo.~Array();
                p->m_locvars.~Array();
                p->m_upvalnames.~Array();
                cnt--;
                if (cnt == 0) {
                    if (ps.length() == 0) {
                        break;
                    } else {
                        p = ps.pop();
                        cnt = cnts.pop();
                        free(p->m_functions.m_items);
                        p->m_functions.m_length = 0;
                    }
                } else {
                    p++;
                }
            }
        }
        free(m_items);
    }
}
