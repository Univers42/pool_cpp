#!/usr/bin/env python3
"""
Class Generator - writes the boilerplate every 42 C++ class repeats.

The piscine asks for a lot of classes and almost all of each one is dictated:
the 42 header block, the include guard spelled after the path, Orthodox
Canonical Form, getters that return the member, a free operator<< that is not
a friend, a virtual destructor the moment anything derives from you. This
writes all of that so the only thing left to type is the part that is actually
about the exercise.

  ./gen_class.py                      interactive wizard (the usual way)
  ./gen_class.py Bureaucrat           wizard, class name pre-filled
  ./gen_class.py --help               every flag, for scripting
  ./gen_class.py --self-test          generate one of each kind and compile it

Everything the wizard asks can also be a flag, so once you know what you want:

  ./gen_class.py Animal --kind abstract --attr 'std::string type' \\
      --pure 'void makeSound() const' --ostream
  ./gen_class.py Dog --kind derived --base Animal --attr 'Brain* brain'

Generated code is C++98 and compiles clean under
-Wall -Wextra -Werror -pedantic-errors; --self-test is what keeps that true.
"""

import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from datetime import datetime
from pathlib import Path

RESET = '\033[0m'
BOLD = '\033[1m'
DIM = '\033[2m'
GREEN = '\033[92m'
CYAN = '\033[96m'
YELLOW = '\033[93m'
RED = '\033[91m'

# ── the 42 header block ──────────────────────────────────────────────────────
# Each line is exactly 80 characters: a frame of "/*" + spaces + "*/" with the
# ASCII art and the text overlaid at fixed columns. The columns below were
# measured off an existing header rather than guessed, and _frame() asserts the
# width so a long login can never silently push the block out of shape.

_ART = [
    (2, None),                              # 0: filled with asterisks
    (None, None),                           # 1: blank
    (58, ':::      ::::::::'),              # 2
    (56, ':+:      :+:    :+:'),            # 3: + filename
    (54, '+:+ +:+         +:+'),            # 4
    (50, '+#+  +:+       +#+'),             # 5: + "By: login <email>"
    (50, '+#+#+#+#+#+   +#+'),              # 6
    (55, '#+#    #+#'),                     # 7: + "Created: ... by login"
    (54, '###   ########.fr'),              # 8: + "Updated: ... by login"
    (None, None),                           # 9: blank
    (2, None),                              # 10: filled with asterisks
]

TEXT_COL = 5


def _frame(art_col=None, art=None, text=None):
    """One 80-character header line: art and text overlaid on a blank frame."""
    buf = list('/*' + ' ' * 76 + '*/')
    if text is not None:
        # A long login would run into the ASCII art and silently overwrite it,
        # so it gets cut instead: a truncated name is obvious, a mangled 42
        # header is the kind of thing nobody notices until the defence.
        limit = (art_col if art_col is not None else 78) - TEXT_COL - 1
        if len(text) > limit:
            text = text[:limit]
        for i, ch in enumerate(text):
            buf[TEXT_COL + i] = ch
    if art is not None:
        for i, ch in enumerate(art):
            buf[art_col + i] = ch
    line = ''.join(buf)
    assert len(line) == 80, 'header line is %d chars, not 80: %r' % (
        len(line), line)
    return line


def header_block(filename, login, email, created=None, updated=None):
    now = datetime.now().strftime('%Y/%m/%d %H:%M:%S')
    created = created or now
    updated = updated or now
    rule = '/* ' + '*' * 74 + ' */'
    return '\n'.join([
        rule,
        _frame(),
        _frame(58, ':::      ::::::::'),
        _frame(56, ':+:      :+:    :+:', filename),
        _frame(54, '+:+ +:+         +:+'),
        _frame(50, '+#+  +:+       +#+', 'By: %s <%s>' % (login, email)),
        _frame(50, '+#+#+#+#+#+   +#+'),
        _frame(55, '#+#    #+#', 'Created: %s by %s' % (created, login)),
        _frame(54, '###   ########.fr', 'Updated: %s by %s' % (updated, login)),
        _frame(),
        rule,
        '',
    ])


BY_LINE_RE = re.compile(r'^/\*   By: (\S+) <([^>]+)>')


def identity(directory='.'):
    """The login and email to stamp into the header block.

    Copied from a neighbouring file first. A repo full of headers saying
    `dlesieur <dlesieur@student.42.fr>` should not suddenly grow one saying
    whatever `git config user.name` happens to be on this machine - the point
    of the block is that every file in the turn-in agrees.
    """
    here = Path(directory).resolve()
    for folder in [here] + list(here.parents)[:4]:
        for path in sorted(folder.glob('*.hpp')) + sorted(folder.glob('*.cpp')):
            try:
                first_lines = path.read_text().split('\n')[:8]
            except (OSError, UnicodeDecodeError):
                continue
            for line in first_lines:
                match = BY_LINE_RE.match(line)
                if match:
                    return match.group(1), match.group(2)

    # Nothing to copy: $USER is the 42 login on a school machine.
    login = os.environ.get('USER') or 'student'
    email = None
    try:
        value = subprocess.check_output(
            ['git', 'config', '--get', 'user.email'],
            stderr=subprocess.DEVNULL).decode().strip()
        if value.endswith('@student.42.fr'):
            email = value
            login = value.split('@')[0]
    except Exception:
        pass
    return login, email or '%s@student.42.fr' % login


# ── include guards ───────────────────────────────────────────────────────────
def guard_for(path):
    """CPP_MODULE08_EX01_SPAN_HPP_ from cpp_module08/ex01/Span.hpp."""
    path = Path(path).resolve()
    parts = []
    for part in path.parent.parts:
        if re.match(r'^cpp_module\d+$', part) or re.match(r'^ex\d+$', part):
            parts.append(part)
    parts.append(path.stem)
    guard = '_'.join(parts) + '_' + path.suffix.lstrip('.') + '_'
    return re.sub(r'[^A-Za-z0-9]', '_', guard).upper()


# ── the pieces of a class ────────────────────────────────────────────────────
PRIMITIVES = {
    'bool', 'char', 'signed char', 'unsigned char', 'short', 'unsigned short',
    'int', 'unsigned', 'unsigned int', 'long', 'unsigned long', 'float',
    'double', 'long double', 'size_t', 'std::size_t', 'ptrdiff_t',
    'std::ptrdiff_t',
}


class Attribute(object):
    """One data member, plus the getter/setter it implies."""

    def __init__(self, spec):
        text = spec.strip().rstrip(';').strip()
        if not text:
            raise ValueError('empty attribute')
        self.is_const = text.startswith('const ')
        body = text[6:].strip() if self.is_const else text
        tokens = body.split()
        if len(tokens) < 2:
            raise ValueError(
                "attribute needs a type and a name, e.g. 'std::string name': %r"
                % spec)
        name = tokens[-1]
        type_ = ' '.join(tokens[:-1])
        # 'Brain *brain' and 'Brain* brain' mean the same thing.
        while name.startswith('*') or name.startswith('&'):
            type_ += name[0]
            name = name[1:]
        self.name = name
        self.type = type_
        self.member = '_' + name

    @property
    def is_by_value(self):
        return self.type in PRIMITIVES or self.type.endswith('*')

    @property
    def getter(self):
        # A bool already named like a question reads worse with get- bolted on:
        # isSigned() beats getIsSigned(), and the standard library agrees.
        if self.type == 'bool' and re.match(r'^(is|has|can|should)[A-Z]',
                                            self.name):
            return self.name
        return 'get' + self.name[0].upper() + self.name[1:]

    @property
    def setter(self):
        return 'set' + self.name[0].upper() + self.name[1:]

    @property
    def getter_return(self):
        return self.type if self.is_by_value else 'const %s&' % self.type

    @property
    def param(self):
        return ('%s %s' % (self.type, self.name) if self.is_by_value
                else 'const %s& %s' % (self.type, self.name))

    def declaration(self):
        return '  %s%s %s;' % ('const ' if self.is_const else '',
                               self.type, self.member)


METHOD_RE = re.compile(
    r'^\s*(?P<static>static\s+)?(?P<virtual>virtual\s+)?'
    r'(?P<ret>.+?)\s+(?P<name>[A-Za-z_]\w*)\s*\((?P<args>[^)]*)\)\s*'
    r'(?P<const>const)?\s*(?P<pure>=\s*0)?\s*;?\s*$')


class Method(object):
    """One member function, parsed from a signature the user typed."""

    def __init__(self, spec, force_virtual=False, force_pure=False):
        match = METHOD_RE.match(spec.strip())
        if not match:
            raise ValueError(
                "cannot parse method %r - try 'void speak() const'" % spec)
        self.is_static = bool(match.group('static'))
        self.is_pure = force_pure or bool(match.group('pure'))
        self.is_virtual = (force_virtual or self.is_pure
                           or bool(match.group('virtual')))
        self.ret = match.group('ret').strip()
        self.name = match.group('name')
        self.args = match.group('args').strip()
        self.is_const = bool(match.group('const'))
        if self.is_static and self.is_virtual:
            raise ValueError('a method cannot be both static and virtual')

    def param_list(self):
        return [a.strip() for a in self.args.split(',') if a.strip()]

    def declaration(self):
        prefix = 'static ' if self.is_static else ('virtual ' if self.is_virtual
                                                   else '')
        suffix = (' const' if self.is_const else '') + \
            (' = 0' if self.is_pure else '') + ';'
        return '\n'.join(_wrap_call('%s%s %s' % (prefix, self.ret, self.name),
                                    self.param_list(), suffix, '  '))

    def arg_names(self):
        """Parameter names, for the (void) casts that silence -Wunused."""
        names = []
        for arg in self.args.split(','):
            arg = arg.strip()
            if not arg or arg == 'void':
                continue
            token = arg.split('=')[0].strip().split()[-1]
            token = token.lstrip('*&').split('[')[0]
            if re.match(r'^[A-Za-z_]\w*$', token) and token not in (
                    'const', 'unsigned', 'signed', 'long', 'short'):
                names.append(token)
        return names


def stub_body(ret, indent='  '):
    """A body that compiles: unused args cast away, a value returned."""
    ret = ret.strip()
    if ret in ('void', ''):
        return []
    if ret.endswith('&'):
        base = ret[:-1].strip()
        if base.startswith('const '):
            base = base[6:].strip()
        return [
            '%sstatic %s placeholder;' % (indent, base),
            '%sreturn placeholder;' % indent,
        ]
    if ret.endswith('*'):
        return ['%sreturn NULL;' % indent]
    if ret == 'bool':
        return ['%sreturn false;' % indent]
    if ret in PRIMITIVES:
        return ['%sreturn 0;' % indent]
    return ['%sreturn %s();' % (indent, ret)]


# ── the specification of one class ───────────────────────────────────────────
KINDS = {
    'base': 'a plain concrete class in Orthodox Canonical Form',
    'polymorphic': 'a concrete class meant to be inherited from (virtual dtor)',
    'abstract': 'cannot be instantiated: at least one pure virtual member',
    'interface': 'all pure virtual, no data, header-only (the AMateria shape)',
    'derived': 'inherits from a base class you name',
    'template': 'a class template, header-only as C++98 requires',
    'utility': 'non-instantiable, all static (the ScalarConverter shape)',
    'exception': 'derives from std::exception with a what() override',
}


class ClassSpec(object):
    def __init__(self, name, kind='base', base=None, base_header=None,
                 attrs=None, methods=None, exceptions=None, ostream=False,
                 value_ctor=True, setters=True, template_params=None,
                 namespace=None, protected_pure=False):
        self.name = name
        self.kind = kind
        self.base = base
        self.base_header = base_header or (base + '.hpp' if base else None)
        self.attrs = attrs or []
        self.methods = methods or []
        self.exceptions = exceptions or []
        self.ostream = ostream
        self.value_ctor = value_ctor and bool(self.attrs)
        self.setters = setters
        self.template_params = template_params or ['typename T']
        self.namespace = namespace
        # Pure virtuals are public by default - Animal::makeSound and
        # AMateria::use are the common shape. Protected is the template-method
        # variant, where a public member validates and then delegates.
        self.protected_pure = protected_pure

    # -- properties the kind implies -----------------------------------------
    @property
    def header_only(self):
        return self.kind in ('interface', 'template')

    @property
    def virtual_dtor(self):
        return self.kind in ('polymorphic', 'abstract', 'interface', 'derived')

    @property
    def has_ocf(self):
        return self.kind not in ('interface', 'utility')

    @property
    def pure_methods(self):
        return [m for m in self.methods if m.is_pure]

    @property
    def concrete_methods(self):
        return [m for m in self.methods if not m.is_pure]

    @property
    def template_head(self):
        return 'template <%s>' % ', '.join(self.template_params)

    @property
    def qualified(self):
        if self.kind == 'template':
            names = [p.split()[-1] for p in self.template_params]
            return '%s<%s>' % (self.name, ', '.join(names))
        return self.name

    def _user_types(self, blob):
        """Class-looking type names in a blob of declarations.

        Anything capitalised that is not in std:: and not this class or its
        base. These are the types a generated header would otherwise mention
        without declaring - the reason the first AForm this tool produced did
        not compile.
        """
        names = set()
        for token in re.findall(r'\b[A-Z]\w*\b', blob):
            if token in (self.name, self.base):
                continue
            names.add(token)
        for stdname in re.findall(r'std::(\w+)', blob):
            names.discard(stdname)
        # Template parameters are not types to declare.
        for param in self.template_params:
            names.discard(param.split()[-1])
        return names

    def forward_declarations(self):
        """Types used only through a reference or pointer: `class X;` is enough."""
        blob = ' '.join(m.ret + ' ' + m.args for m in self.methods)
        blob += ' ' + ' '.join(a.type for a in self.attrs if
                               a.type.endswith('*'))
        return sorted(self._user_types(blob))

    def sibling_includes(self):
        """Types stored by value: a forward declaration will not do."""
        blob = ' '.join(a.type for a in self.attrs
                        if not a.type.endswith('*'))
        return sorted(self._user_types(blob))

    def includes(self):
        found = set()
        blob = ' '.join(
            [a.type for a in self.attrs] +
            [m.ret + ' ' + m.args for m in self.methods])
        if 'std::string' in blob or self.kind == 'exception':
            found.add('<string>')
        if 'std::vector' in blob:
            found.add('<vector>')
        if 'std::list' in blob:
            found.add('<list>')
        if 'std::map' in blob:
            found.add('<map>')
        if 'std::deque' in blob:
            found.add('<deque>')
        if self.ostream:
            found.add('<ostream>')
        if self.exceptions or self.kind == 'exception':
            found.add('<exception>')
        if self.kind == 'exception':
            found.discard('<string>')
        return sorted(found)


# ── header emission ──────────────────────────────────────────────────────────
def emit_hpp(spec, path, login, email):
    out = [header_block(path.name, login, email)]
    guard = guard_for(path)
    out.append('#ifndef %s' % guard)
    out.append('#define %s' % guard)
    out.append('')

    for inc in spec.includes():
        out.append('#include %s' % inc)
    own = ['"%s"' % spec.base_header] if spec.base else []
    own += ['"%s.hpp"' % name for name in spec.sibling_includes()]
    if own:
        if spec.includes():
            out.append('')
        for inc in sorted(set(own)):
            out.append('#include %s' % inc)
    if spec.includes() or own:
        out.append('')

    # A reference or a pointer only needs the name, not the definition. Doing
    # it this way keeps headers from including each other in a cycle, which is
    # exactly what happens when Bureaucrat and AForm each take the other.
    forwards = spec.forward_declarations()
    if forwards:
        for name in forwards:
            out.append('class %s;' % name)
        out.append('')

    if spec.namespace:
        out.append('namespace %s {' % spec.namespace)
        out.append('')

    body = _class_body(spec)
    out.extend(body)
    out.append('')

    if spec.ostream:
        decl = 'std::ostream& operator<<(std::ostream& os, const %s& value);' \
            % spec.qualified
        if spec.header_only:
            out.extend(_ostream_definition(spec, inline=True))
        else:
            out.append('// A free operator<<, not a member and not a friend:')
            out.append('// the getters are public, so it needs no privileged'
                       ' access. The 42')
            out.append('// evaluation sheet flags the friend keyword, and this'
                       ' is the case')
            out.append('// where people reach for it.')
            if spec.kind == 'template':
                out.append(spec.template_head)
            out.append(decl)
        out.append('')

    if spec.namespace:
        out.append('}  // namespace %s' % spec.namespace)
        out.append('')

    out.append('#endif  // %s' % guard)
    return '\n'.join(out) + '\n'


def _exception_class(name, indent='  '):
    return [
        '%sclass %s : public std::exception {' % (indent, name),
        '%s public:' % indent,
        '%s  virtual const char* what() const throw();' % indent,
        '%s};' % indent,
    ]


def _class_body(spec):
    out = []
    name = spec.name

    if spec.kind == 'template':
        out.append(spec.template_head)

    if spec.kind == 'exception':
        out.append('class %s : public std::exception {' % name)
        out.append(' public:')
        out.append('  virtual const char* what() const throw();')
        out.append('};')
        return out

    inherit = ''
    if spec.base:
        inherit = ' : public %s' % spec.base
    out.append('class %s%s {' % (name, inherit))
    out.append(' public:')

    # Nested exception classes first: they are part of the interface a caller
    # has to catch, so they belong above the members that throw them.
    if spec.exceptions:
        out.append('  // ponytail: exception classes skip OCF - the subject')
        out.append('  // explicitly allows it.')
        for exc in spec.exceptions:
            out.extend(_exception_class(exc))
        out.append('')

    if spec.kind == 'utility':
        for method in spec.methods:
            method.is_static = True
            out.append(method.declaration())
        if not spec.methods:
            out.append('  static void run();')
        out.append('')
        out.append(' private:')
        out.append('  // Declared, never defined, so an accidental %s u;'
                   % name.lower()[:1])
        out.append('  // fails at link time: this class is not meant to exist.')
        out.append('  %s();' % name)
        out.append('  %s(const %s& other);' % (name, name))
        out.append('  %s& operator=(const %s& other);' % (name, name))
        out.append('  ~%s();' % name)
        out.append('};')
        return out

    if spec.kind == 'interface':
        out.append('  virtual ~%s() {}' % name)
        out.append('')
        for method in spec.methods:
            method.is_virtual = True
            method.is_pure = True
            out.append(method.declaration())
        if not spec.methods:
            out.append('  virtual void execute() = 0;')
        out.append('};')
        return out

    # Orthodox Canonical Form.
    inline = spec.kind == 'template'
    if inline:
        default_inits = ', '.join('%s()' % a.member for a in spec.attrs)
        if default_inits:
            out.extend(_wrap_member('  %s() : %s {}' % (name, default_inits)))
        else:
            out.append('  %s() {}' % name)
        if spec.value_ctor:
            # A class template has no .cpp, so every member must be defined
            # here - declaring this one and stopping leaves it undefined at
            # link time, which is exactly the bug the self-test caught.
            params = ', '.join(a.param for a in spec.attrs)
            inits = ', '.join('%s(%s)' % (a.member, a.name) for a in spec.attrs)
            prefix = 'explicit ' if len(spec.attrs) == 1 else ''
            out.extend(_wrap_member('  %s%s(%s) : %s {}'
                                    % (prefix, name, params, inits)))
        out.append('  %s(const %s& other)%s' % (
            name, spec.qualified, _copy_init(spec, inline=True)))
        out.append('  %s& operator=(const %s& other) {'
                   % (spec.qualified, spec.qualified))
        out.append('    if (this != &other) {')
        for attr in spec.attrs:
            if attr.is_const:
                continue
            out.append('      %s = other.%s;' % (attr.member, attr.member))
        if not [a for a in spec.attrs if not a.is_const]:
            out.append('      // no assignable members yet')
        out.append('    }')
        out.append('    return *this;')
        out.append('  }')
        out.append('  %s~%s() {}' % ('virtual ' if spec.virtual_dtor else '',
                                     name))
    else:
        out.append('  %s();' % name)
        if spec.value_ctor:
            prefix = 'explicit ' if len(spec.attrs) == 1 else ''
            out.extend(_wrap_call('%s%s' % (prefix, name),
                                  [a.param for a in spec.attrs], ';', '  '))
        out.append('  %s(const %s& other);' % (name, name))
        out.append('  %s& operator=(const %s& other);' % (name, name))
        out.append('  %s~%s();' % ('virtual ' if spec.virtual_dtor else '',
                                   name))
    out.append('')

    # Accessors.
    accessors = []
    for attr in spec.attrs:
        accessors.extend(_wrap_call('%s %s' % (attr.getter_return, attr.getter),
                                    [], ' const;', '  '))
        if spec.setters and not attr.is_const:
            accessors.extend(_wrap_call('void %s' % attr.setter, [attr.param],
                                        ';', '  '))
    if accessors:
        if inline:
            accessors = _inline_accessors(spec)
        out.extend(accessors)
        out.append('')

    concrete = [m for m in spec.concrete_methods]
    if concrete:
        for method in concrete:
            if inline:
                out.extend(_inline_method(method))
            else:
                out.append(method.declaration())
        out.append('')

    pure = spec.pure_methods
    if pure:
        if spec.protected_pure:
            # The template-method shape: a public entry point does the shared
            # checks, then delegates to this. Module 05's AForm is the example.
            out.append(' protected:')
        out.append('  // Implemented by every concrete subclass; this class'
                   ' only')
        out.append('  // guarantees that they exist.')
        for method in pure:
            out.append(method.declaration())
        out.append('')
    elif spec.kind == 'abstract':
        # No pure virtual member was given, so make the destructor pure. It
        # still needs an out-of-line definition, which emit_cpp writes.
        for i, line in enumerate(out):
            if line.strip().startswith('virtual ~'):
                out[i] = '  virtual ~%s() = 0;' % name
                break

    if spec.attrs:
        out.append(' private:')
        for attr in spec.attrs:
            out.append(attr.declaration())
    elif out[-1] == '':
        out.pop()

    out.append('};')
    return out


LIMIT = 80


def _wrap_comment(text, indent=''):
    """Break a // comment at the column limit instead of running past it."""
    words = text.split()
    lines = []
    current = '%s//' % indent
    for word in words:
        if len(current) + 1 + len(word) > LIMIT and current.strip() != '//':
            lines.append(current)
            current = '%s//' % indent
        current += ' ' + word
    if current.strip() != '//':
        lines.append(current)
    return lines


def _wrap_call(head, params, tail, indent=''):
    """`head(params)tail`, wrapped at the column limit.

    One line if it fits; then the whole parameter list on a continuation line;
    then one parameter per line. Long member names make all three cases real,
    which is why the self-test carries a class with deliberately long ones.
    """
    one = '%s%s(%s)%s' % (indent, head, ', '.join(params), tail)
    if len(one) <= LIMIT:
        return [one]
    cont = indent + '    '
    joined = ', '.join(params)
    if len(cont) + len(joined) + 1 + len(tail) <= LIMIT:
        return ['%s%s(' % (indent, head), '%s%s)%s' % (cont, joined, tail)]
    out = ['%s%s(' % (indent, head)]
    for i, param in enumerate(params):
        end = ',' if i < len(params) - 1 else ')%s' % tail
        out.append('%s%s%s' % (cont, param, end))
    return out


def _emit_function(ret, qualifier, name, params, suffix, body, indent=''):
    """A complete function definition, kept inside the column limit."""
    head = ('%s %s%s' % (ret, qualifier, name)) if ret else \
        ('%s%s' % (qualifier, name))
    signature = _wrap_call(head, params, suffix, indent)
    # A one-line body collapses into the signature - but never a comment, or
    # the closing brace ends up commented out along with it.
    if (len(signature) == 1 and len(body) == 1
            and not body[0].strip().startswith('//')):
        one = '%s { %s }' % (signature[0], body[0].strip())
        if len(one) <= LIMIT:
            return [one]
    out = list(signature)
    if len(out[-1]) + 2 <= LIMIT:
        out[-1] += ' {'
    else:
        # A long qualified name can leave no room for the brace; giving it its
        # own line is uglier than wrapping but it is still inside the limit.
        out.append('%s{' % indent)
    out.extend('%s  %s' % (indent, line.strip()) for line in body)
    out.append('%s}' % indent)
    return out


def _wrap_ctor(head, params, parts, tail=' {}', indent=''):
    """A constructor definition wrapped the way clang-format would."""
    signature = _wrap_call(head, params, '', indent)
    if not parts:
        if len(signature) == 1:
            return ['%s%s' % (signature[0], tail)]
        signature[-1] += tail
        return signature
    joined = ', '.join(parts)
    if len(signature) == 1:
        one_line = '%s : %s%s' % (signature[0], joined, tail)
        if len(one_line) <= LIMIT:
            return [one_line]
        two_line = '%s    : %s%s' % (indent, joined, tail)
        if len(two_line) <= LIMIT:
            return [signature[0], two_line]
    out = list(signature)
    for i, part in enumerate(parts):
        lead = '%s    : ' % indent if i == 0 else '%s      ' % indent
        end = tail if i == len(parts) - 1 else ','
        out.append('%s%s%s' % (lead, part, end))
    return out


def _wrap_member(line):
    """Keep an inline member definition inside the column limit."""
    if len(line) <= LIMIT:
        return [line]
    head, _, tail = line.partition(' : ')
    parts = [p.strip() for p in tail.rstrip(' {}').split(', ')]
    open_paren = head.index('(')
    return _wrap_ctor(head[:open_paren].strip(),
                      [p.strip() for p in
                       head[open_paren + 1:].rstrip(')').split(', ') if p.strip()],
                      parts, ' {}', '  ')


def _value_ctor_signature(spec):
    params = ', '.join(a.param for a in spec.attrs)
    # A single-argument constructor converts implicitly unless it is explicit,
    # which is almost never what you want.
    prefix = 'explicit ' if len(spec.attrs) == 1 else ''
    return '%s%s(%s)' % (prefix, spec.name, params)


def _copy_init(spec, inline=False):
    parts = []
    if spec.base:
        parts.append('%s(other)' % spec.base)
    parts.extend('%s(other.%s)' % (a.member, a.member) for a in spec.attrs)
    if not parts:
        return ' {}' if inline else ';'
    joined = ', '.join(parts)
    return ' : %s {}' % joined if inline else ' : %s {}' % joined


def _inline_accessors(spec):
    out = []
    for attr in spec.attrs:
        out.append('  %s %s() const { return %s; }' % (
            attr.getter_return, attr.getter, attr.member))
        if spec.setters and not attr.is_const:
            out.append('  void %s(%s) { %s = %s; }' % (
                attr.setter, attr.param, attr.member, attr.name))
    return out


def _inline_method(method):
    out = [method.declaration().rstrip(';') + ' {']
    for arg in method.arg_names():
        out.append('    (void)%s;' % arg)
    out.extend(stub_body(method.ret, '    '))
    out.append('  }')
    return out


def _ostream_definition(spec, inline=False):
    out = []
    if spec.kind == 'template':
        out.append(spec.template_head)
    out.append('std::ostream& operator<<(std::ostream& os, const %s& value) {'
               % spec.qualified)
    if spec.attrs:
        out.append('  os << "%s(";' % spec.name)
        for i, attr in enumerate(spec.attrs):
            label = ('%s=' % attr.name) if i == 0 else (', %s=' % attr.name)
            line = '  os << "%s" << value.%s();' % (label, attr.getter)
            if len(line) <= 80:
                out.append(line)
            else:
                out.append('  os << "%s"' % label)
                out.append('     << value.%s();' % attr.getter)
        out.append('  return os << ")";')
    else:
        out.append('  (void)value;')
        out.append('  return os << "%s()";' % spec.name)
    out.append('}')
    return out


# ── source emission ──────────────────────────────────────────────────────────
def emit_cpp(spec, hpp_name, path, login, email):
    out = [header_block(path.name, login, email)]
    out.append('#include "%s"' % hpp_name)
    out.append('')

    if spec.namespace:
        out.append('namespace %s {' % spec.namespace)
        out.append('')

    name = spec.name

    if spec.kind == 'exception':
        out.append('const char* %s::what() const throw() {' % name)
        out.append('  return "%s";' % name)
        out.append('}')
        out.append('')
        if spec.namespace:
            out.append('}  // namespace %s' % spec.namespace)
            out.append('')
        return '\n'.join(out)

    for exc in spec.exceptions:
        out.extend(_emit_function(
            'const char*', '%s::%s::' % (name, exc), 'what', [],
            ' const throw()', ['return "%s: %s";' % (name, exc)]))
        out.append('')

    if spec.kind == 'utility':
        methods = spec.methods or [Method('void run()')]
        for method in methods:
            out.extend(_out_of_line(spec, method))
            out.append('')
        if spec.namespace:
            out.append('}  // namespace %s' % spec.namespace)
            out.append('')
        return '\n'.join(out)

    # Default constructor. Every member is value-initialised explicitly:
    # leaving an int member out of the list leaves it holding garbage, which is
    # the single most common way a default-constructed 42 class misbehaves.
    qualified = '%s::%s' % (name, name)
    out.extend(_wrap_ctor(qualified, [],
                          ['%s()' % a.member for a in spec.attrs]))
    out.append('')

    if spec.value_ctor:
        out.extend(_wrap_ctor(
            qualified, [a.param for a in spec.attrs],
            ['%s(%s)' % (a.member, a.name) for a in spec.attrs]))
        out.append('')

    # Copy constructor.
    copy_parts = []
    if spec.base:
        copy_parts.append('%s(other)' % spec.base)
    copy_parts.extend('%s(other.%s)' % (a.member, a.member) for a in spec.attrs)
    if copy_parts:
        out.extend(_wrap_ctor(qualified, ['const %s& other' % name],
                              copy_parts))
    else:
        out.extend(_emit_function('', '', qualified, ['const %s& other' % name],
                                  '', ['(void)other;']))
    out.append('')

    # Copy assignment: clone first, delete second, assign last is the repo's
    # rule for owning classes; with value members the guard plus member-wise
    # assignment is all it takes.
    assignable = [a for a in spec.attrs if not a.is_const]
    const_attrs = [a for a in spec.attrs if a.is_const]
    signature = '%s& %s::operator=(const %s& other)' % (name, name, name)
    if len(signature) + 2 <= 80:
        out.append('%s {' % signature)
    else:
        out.append('%s& %s::operator=(' % (name, name))
        out.append('    const %s& other) {' % name)
    if const_attrs:
        members = ', '.join(a.member for a in const_attrs)
        verb = 'is' if len(const_attrs) == 1 else 'are'
        pronoun = 'it' if len(const_attrs) == 1 else 'they'
        note = _wrap_comment('%s %s const, so %s cannot be assigned.'
                             % (members, verb, pronoun))
        for offset, line in enumerate(note):
            out.insert(len(out) - 1 + offset, line)
    body = []
    if spec.base:
        body.append('    %s::operator=(other);' % spec.base)
    for attr in assignable:
        body.append('    %s = other.%s;' % (attr.member, attr.member))
    if body:
        out.append('  if (this != &other) {')
        out.extend(body)
        out.append('  }')
    else:
        out.append('  (void)other;')
    out.append('  return *this;')
    out.append('}')
    out.append('')

    # Destructor.
    if spec.kind == 'abstract' and not spec.pure_methods:
        out.append('// A pure virtual destructor still needs a definition:'
                   ' every subclass')
        out.append('// destructor calls it. Declaring it = 0 is what makes'
                   ' this class')
        out.append('// abstract when no other member is pure virtual.')
    out.append('%s::~%s() {}' % (name, name))
    out.append('')

    # Accessors.
    for attr in spec.attrs:
        out.extend(_emit_function(attr.getter_return, '%s::' % name,
                                  attr.getter, [], ' const',
                                  ['return %s;' % attr.member]))
        if spec.setters and not attr.is_const:
            out.append('')
            out.extend(_emit_function(
                'void', '%s::' % name, attr.setter, [attr.param], '',
                ['%s = %s;' % (attr.member, attr.name)]))
        out.append('')

    for method in spec.concrete_methods:
        out.extend(_out_of_line(spec, method))
        out.append('')

    if spec.ostream:
        out.extend(_ostream_definition(spec))
        out.append('')

    if spec.namespace:
        out.append('}  // namespace %s' % spec.namespace)
        out.append('')

    return '\n'.join(out)


def _out_of_line(spec, method):
    """A method definition with a body that compiles and a TODO that doesn't lie."""
    body = ['// TODO: %s' % method.name]
    body += ['(void)%s;' % arg for arg in method.arg_names()]
    body += [line.strip() for line in stub_body(method.ret)]
    return _emit_function(method.ret, '%s::' % spec.name, method.name,
                          method.param_list(),
                          ' const' if method.is_const else '', body)


# ── the test skeleton ────────────────────────────────────────────────────────
def emit_test(spec, path, login, email):
    """The canonical-form assertions every class needs, pre-written."""
    name = spec.name
    instantiable = spec.kind in ('base', 'polymorphic', 'derived', 'template')
    out = [header_block(path.name, login, email)]
    out.append('// Assertions for %s. The canonical-form section below is the'
               % name)
    out.append('// part that is the same for every class in the piscine, so it'
               ' is written')
    out.append('// out already; everything under "behaviour" is yours.')
    out.append('//')
    out.append('//   make test        build and run')
    out.append('//   valgrind --leak-check=full ./build/bin/%s_test' % name.lower())
    out.append('')
    out.append('#include <cassert>')
    out.append('#include <iostream>')
    out.append('#include <sstream>')
    out.append('')
    out.append('#include "../%s.hpp"' % name)
    out.append('')
    type_name = spec.qualified if spec.kind != 'template' else '%s<int>' % name

    if not instantiable:
        out.append('// %s is %s, so it is exercised through a subclass.' % (
            name, KINDS[spec.kind]))
        out.append('int main() {')
        out.append('  std::cout << "%s: nothing to instantiate directly"'
                   % name)
        out.append('            << std::endl;')
        out.append('  return 0;')
        out.append('}')
        return '\n'.join(out) + '\n'

    out.append('static void test_canonical_form() {')
    out.append('  %s original;' % type_name)
    out.append('')
    width = max(len('  %s copy(original);' % type_name),
                len('  %s assigned;' % type_name),
                len('  assigned = original;')) + 2
    for code, note in (('  %s copy(original);' % type_name, 'copy constructor'),
                       ('  %s assigned;' % type_name, 'default constructor'),
                       ('  assigned = original;', 'copy assignment')):
        out.append('%s// %s' % (code.ljust(width), note))
    out.append('')
    for attr in spec.attrs:
        out.append('  assert(copy.%s() == original.%s());'
                   % (attr.getter, attr.getter))
        out.append('  assert(assigned.%s() == original.%s());'
                   % (attr.getter, attr.getter))
    out.append('')
    out.append('  // Self-assignment through an alias, so the compiler cannot')
    out.append('  // fold it away and -Wself-assign-overloaded stays quiet.')
    out.append('  %s& alias = original;' % type_name)
    out.append('  original = alias;')
    settable = [a for a in spec.attrs if not a.is_const]
    if settable and spec.setters:
        attr = settable[0]
        out.append('')
        out.append('  // Independence: mutating a copy must not reach the'
                   ' original.')
        out.append('  // TODO: pick a value that is actually different here.')
        out.append('  // copy.%s(...);' % attr.setter)
        out.append('  // assert(original.%s() != copy.%s());'
                   % (attr.getter, attr.getter))
    out.append('}')
    out.append('')

    if spec.ostream:
        out.append('static void test_stream_output() {')
        out.append('  %s value;' % type_name)
        out.append('  std::ostringstream out;')
        out.append('  out << value;')
        out.append('  assert(!out.str().empty());')
        out.append('  // TODO: assert the exact format once it is decided.')
        out.append('}')
        out.append('')

    if spec.exceptions:
        out.append('static void test_exceptions() {')
        for exc in spec.exceptions:
            out.append('  // TODO: drive %s into throwing %s.' % (name, exc))
            out.append('  //   bool threw = false;')
            out.append('  //   try { ... } catch (const %s::%s&) { threw = true; }'
                       % (name, exc))
            out.append('  //   assert(threw);')
        out.append('}')
        out.append('')

    out.append('static void test_behaviour() {')
    out.append('  // TODO: the part that is actually about the exercise.')
    out.append('}')
    out.append('')
    out.append('int main() {')
    out.append('  test_canonical_form();')
    if spec.ostream:
        out.append('  test_stream_output();')
    if spec.exceptions:
        out.append('  test_exceptions();')
    out.append('  test_behaviour();')
    out.append('')
    out.append('  std::cout << "%s: all tests passed" << std::endl;' % name)
    out.append('  return 0;')
    out.append('}')
    return '\n'.join(out) + '\n'


# ── writing files ────────────────────────────────────────────────────────────
def write_file(path, content, force=False, dry_run=False):
    if dry_run:
        print('%s%s--- %s%s' % (BOLD, CYAN, path, RESET))
        print(content)
        return True
    if path.exists() and not force:
        print('%s  exists   %s%s  (use --force to overwrite)'
              % (YELLOW, path, RESET))
        return False
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content)
    print('%s  wrote    %s%s%s' % (GREEN, RESET, path, RESET))
    return True


def update_makefile(directory, new_source):
    """Add a .cpp to SRC_LIST in place.

    Deliberately surgical rather than a gen_make.py regeneration: regenerating
    overwrites hand-edits, and several exercises in this repo have them
    (module 09's EXEC_BIN names, module 08's test/quick/verify targets).
    """
    makefile = directory / 'Makefile'
    if not makefile.exists():
        print('%s  skipped  no Makefile in %s%s' % (YELLOW, directory, RESET))
        return False
    text = makefile.read_text()
    match = re.search(r'^SRC_LIST = (.*)$', text, re.MULTILINE)
    if not match:
        print('%s  skipped  no SRC_LIST line in %s%s'
              % (YELLOW, makefile, RESET))
        return False
    sources = match.group(1).split()
    if new_source in sources:
        return True
    # Class files sorted, main.cpp last - the convention already in the repo.
    mains = [s for s in sources if s in ('main.cpp', 'Main.cpp')]
    others = sorted([s for s in sources if s not in mains] + [new_source])
    text = text[:match.start(1)] + ' '.join(others + mains) + text[match.end(1):]
    makefile.write_text(text)
    print('%s  updated  %s%s  (SRC_LIST += %s)'
          % (GREEN, RESET, makefile, new_source))
    return True


def generate(spec, directory, login, email, force=False, dry_run=False,
             with_test=False, touch_makefile=False):
    directory = Path(directory)
    hpp_path = directory / ('%s.hpp' % spec.name)
    written = []

    if write_file(hpp_path, emit_hpp(spec, hpp_path, login, email), force,
                  dry_run):
        written.append(hpp_path)

    if not spec.header_only:
        cpp_path = directory / ('%s.cpp' % spec.name)
        if write_file(cpp_path,
                      emit_cpp(spec, hpp_path.name, cpp_path, login, email),
                      force, dry_run):
            written.append(cpp_path)
            if touch_makefile and not dry_run:
                update_makefile(directory, cpp_path.name)

    if with_test:
        test_path = directory / 'tests' / ('%s_test.cpp' % spec.name.lower())
        if write_file(test_path, emit_test(spec, test_path, login, email),
                      force, dry_run):
            written.append(test_path)

    return written


# ── the wizard ───────────────────────────────────────────────────────────────
def ask(prompt, default=None, choices=None):
    suffix = ''
    if default is not None:
        suffix = ' %s[%s]%s' % (DIM, default, RESET)
    while True:
        try:
            answer = input('%s%s%s%s: ' % (CYAN, prompt, RESET, suffix)).strip()
        except EOFError:
            print()
            return default or ''
        if not answer and default is not None:
            return default
        if not answer:
            continue
        if choices and answer not in choices:
            print('%s  pick one of: %s%s' % (RED, ', '.join(choices), RESET))
            continue
        return answer


def ask_yes(prompt, default=True):
    answer = ask(prompt + ' (y/n)', 'y' if default else 'n')
    return answer.lower().startswith('y')


def ask_list(prompt, example):
    print('%s%s%s %s(one per line, blank to finish, e.g. %s)%s'
          % (CYAN, prompt, RESET, DIM, example, RESET))
    items = []
    while True:
        try:
            line = input('  %s>%s ' % (DIM, RESET)).strip()
        except EOFError:
            print()
            break
        if not line:
            break
        items.append(line)
    return items


def wizard(preset_name=None, directory='.'):
    print('%s%sC++ class generator%s  %s- 42 header, guard, OCF, the lot%s'
          % (BOLD, CYAN, RESET, DIM, RESET))
    print('%s  target directory: %s%s\n' % (DIM, Path(directory).resolve(),
                                            RESET))

    name = preset_name or ask('Class name')

    print('\n%sKinds:%s' % (BOLD, RESET))
    for key, description in KINDS.items():
        print('  %s%-12s%s %s%s%s' % (GREEN, key, RESET, DIM, description,
                                      RESET))
    print()
    kind = ask('Kind', 'base', list(KINDS.keys()))

    base = None
    base_header = None
    if kind == 'derived':
        base = ask('Base class name')
        base_header = ask('Base header', '%s.hpp' % base)

    template_params = None
    if kind == 'template':
        params = ask('Template parameters', 'typename T')
        template_params = [p.strip() for p in params.split(',')]

    attrs = []
    if kind not in ('interface', 'utility', 'exception'):
        for line in ask_list('Attributes', "std::string name  /  const int grade"):
            try:
                attrs.append(Attribute(line))
            except ValueError as error:
                print('%s  %s%s' % (RED, error, RESET))

    methods = []
    hint = "void speak() const  /  virtual int area() const = 0"
    if kind == 'derived':
        # An override is virtual whether or not you say so, but writing it
        # keeps the reader from having to open the base to find out.
        hint = "virtual void makeSound() const   (write virtual on overrides)"
    if kind == 'interface':
        hint = 'void use(ICharacter& target)   (all become pure virtual)'
    elif kind == 'utility':
        hint = 'void convert(const std::string& literal)   (all become static)'
    if kind != 'exception':
        for line in ask_list('Methods', hint):
            try:
                methods.append(Method(line))
            except ValueError as error:
                print('%s  %s%s' % (RED, error, RESET))

    protected_pure = False
    if any(m.is_pure for m in methods):
        print('%s  public pure virtuals is the usual shape (Animal::makeSound).'
              % DIM)
        print('  protected is the template-method one, where a public member'
              ' validates')
        print('  and then delegates (module 05 AForm::executeAction).%s' % RESET)
        protected_pure = ask_yes('Put the pure virtuals under protected:?',
                                 False)
    elif kind == 'abstract':
        print('%s  no pure virtual member given - the destructor will be made'
              % YELLOW)
        print('  pure instead, which is the other way to make a class'
              ' abstract.%s' % RESET)

    exceptions = []
    if kind not in ('interface', 'exception', 'utility'):
        if ask_yes('Nested exception classes?', False):
            exceptions = ask_list('Exception class names',
                                  'GradeTooHighException')

    ostream = False
    setters = True
    value_ctor = True
    if kind not in ('interface', 'utility', 'exception'):
        ostream = ask_yes('Free operator<<?', True)
        if attrs:
            value_ctor = ask_yes('Constructor taking every attribute?', True)
            setters = ask_yes('Setters as well as getters?', True)

    with_test = ask_yes('Test skeleton in tests/?', False)
    touch_makefile = ask_yes("Add the .cpp to the Makefile's SRC_LIST?", True)

    spec = ClassSpec(name=name, kind=kind, base=base, base_header=base_header,
                     attrs=attrs, methods=methods, exceptions=exceptions,
                     ostream=ostream, value_ctor=value_ctor, setters=setters,
                     template_params=template_params,
                     protected_pure=protected_pure)
    return spec, with_test, touch_makefile


# ── self-test ────────────────────────────────────────────────────────────────
SELF_TEST_KINDS = [
    ('Plain', dict(kind='base', attrs=['std::string name', 'int weight'],
                   methods=['void describe() const'], ostream=True)),
    ('Machine', dict(kind='polymorphic', attrs=['int serial'],
                     methods=['void start()'], ostream=True)),
    ('AShape', dict(kind='abstract', attrs=['std::string label'],
                    methods=['double area() const = 0', 'void draw() const'],
                    exceptions=['BadShapeException'], ostream=True)),
    ('AEmpty', dict(kind='abstract', attrs=['int tag'])),
    ('IUsable', dict(kind='interface', methods=['void use(int amount)'])),
    ('Converter', dict(kind='utility',
                       methods=['void convert(const std::string& literal)'])),
    ('Box', dict(kind='template', attrs=['T value'], ostream=True,
                 methods=['bool isEmpty() const'])),
    ('BadThing', dict(kind='exception', attrs=[])),
    # Mentions two types it does not own: Tool by reference in a signature,
    # Machine by pointer. Both must end up declared or this header will not
    # compile on its own - the bug the first hand-checked run turned up.
    ('Workshop', dict(kind='base', attrs=['Machine* current', 'int capacity'],
                      methods=['void install(const Tool& tool)'],
                      ostream=True)),
    # Deliberately long names: the initialiser lists here do not fit on one
    # line, and neither does the whole list on a second, so this is what
    # forces the one-initialiser-per-line wrapping to be correct.
    ('RegistrationAuthority',
     dict(kind='base',
          attrs=['const std::string institutionDisplayName',
                 'const std::string accreditationReference',
                 'unsigned int outstandingApplicationCount',
                 'bool hasProvisionalStatus'],
          methods=['void reconcileOutstandingApplications(unsigned int limit)'],
          exceptions=['NotAccreditedException'], ostream=True)),
]


def _build_spec(name, kind, attrs=None, methods=None, exceptions=None,
                ostream=False, base=None, template_params=None):
    return ClassSpec(
        name=name, kind=kind, base=base,
        attrs=[Attribute(a) for a in (attrs or [])],
        methods=[Method(m) for m in (methods or [])],
        exceptions=exceptions or [], ostream=ostream,
        template_params=template_params)


def self_test():
    cxx = os.environ.get('CXX', 'c++')
    flags = ['-std=c++98', '-Wall', '-Wextra', '-Werror', '-pedantic-errors']
    workdir = Path(tempfile.mkdtemp(prefix='gen_class_'))
    login, email = 'dlesieur', 'dlesieur@student.42.fr'
    failures = 0
    total = 0

    print('%s%sgen_class self-test%s  %s- every kind, compiled with %s%s'
          % (BOLD, CYAN, RESET, DIM, ' '.join(flags), RESET))
    print('%s  %s%s\n' % (DIM, workdir, RESET))

    specs = []
    for name, options in SELF_TEST_KINDS:
        spec = _build_spec(name, **options)
        specs.append(spec)
        generate(spec, workdir, login, email, force=True)

    # A derived class on top of the abstract one, which is the pairing that
    # actually exercises base-class initialisation and the override.
    derived = _build_spec('Circle', 'derived', attrs=['double radius'],
                          methods=['double area() const', 'void draw() const'],
                          ostream=True, base='AShape')
    specs.append(derived)
    generate(derived, workdir, login, email, force=True)

    sources = sorted(str(p) for p in workdir.glob('*.cpp'))

    # 1. every header is 80 columns and carries a well-formed 42 block
    for path in sorted(workdir.glob('*.hpp')) + sorted(workdir.glob('*.cpp')):
        total += 1
        lines = path.read_text().split('\n')
        wide = [i + 1 for i, l in enumerate(lines) if len(l) > 80]
        head_ok = (len(lines) > 10 and lines[0].startswith('/* **')
                   and all(len(l) == 80 for l in lines[:11]))
        if wide or not head_ok:
            failures += 1
            print('  %sFAIL%s     %-16s %s' % (
                RED, RESET, path.name,
                'lines over 80: %s' % wide if wide else '42 header malformed'))
        else:
            print('  %spass%s     %-16s %s80 columns, 42 header intact%s'
                  % (GREEN, RESET, path.name, DIM, RESET))

    # 2. a main that uses every generated type has to compile and link
    main = workdir / 'main.cpp'
    main.write_text('''#include <iostream>
#include "AShape.hpp"
#include "BadThing.hpp"
#include "Box.hpp"
#include "Circle.hpp"
#include "Converter.hpp"
#include "IUsable.hpp"
#include "Machine.hpp"
#include "Plain.hpp"

class Tool : public IUsable {
 public:
  virtual ~Tool() {}
  virtual void use(int amount) { (void)amount; }
};

int main() {
  Plain plain("bolt", 3);
  Plain copy(plain);
  copy = plain;
  std::cout << plain << std::endl;

  Machine machine(7);
  std::cout << machine << std::endl;

  Circle circle(1.5);
  const AShape& shape = circle;
  std::cout << shape.getLabel() << " " << shape.area() << std::endl;
  std::cout << circle << std::endl;

  Tool tool;
  IUsable& usable = tool;
  usable.use(1);

  Box<int> box(42);
  Box<int> boxCopy(box);
  boxCopy = box;
  std::cout << box << " " << box.getValue() << std::endl;

  Converter::convert("42");

  try {
    throw BadThing();
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  try {
    throw AShape::BadShapeException();
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
''')
    total += 1
    binary = workdir / 'a.out'
    result = subprocess.run(
        [cxx] + flags + ['-I', str(workdir)] + sources + [str(main),
                                                          '-o', str(binary)],
        capture_output=True, text=True)
    if result.returncode == 0:
        print('  %spass%s     %-16s %sall kinds compile and link together%s'
              % (GREEN, RESET, 'compile', DIM, RESET))
    else:
        failures += 1
        print('  %sFAIL%s     %-16s' % (RED, RESET, 'compile'))
        print('\n'.join('           ' + l
                        for l in result.stderr.split('\n')[:40]))

    # 3. it runs
    total += 1
    if result.returncode == 0:
        run = subprocess.run([str(binary)], capture_output=True, text=True)
        if run.returncode == 0:
            print('  %spass%s     %-16s %sruns clean%s'
                  % (GREEN, RESET, 'execute', DIM, RESET))
        else:
            failures += 1
            print('  %sFAIL%s     %-16s exit %d\n%s'
                  % (RED, RESET, 'execute', run.returncode, run.stderr))
    else:
        failures += 1
        print('  %sskip%s     %-16s (nothing to run)' % (YELLOW, RESET,
                                                         'execute'))

    # 4. no non-template function bodies in the headers: two TUs must link
    for spec in specs:
        if spec.header_only:
            continue
        total += 1
        tu1 = workdir / ('odr1_%s.cpp' % spec.name)
        tu2 = workdir / ('odr2_%s.cpp' % spec.name)
        tu1.write_text('#include "%s.hpp"\nint main() { return 0; }\n'
                       % spec.name)
        tu2.write_text('#include "%s.hpp"\n' % spec.name)
        # Link every generated .cpp, not just this class's: a derived class
        # needs its base's constructors, and an undefined base would look
        # exactly like the ODR violation this check is hunting for.
        odr = subprocess.run(
            [cxx, '-std=c++98', '-I', str(workdir), str(tu1), str(tu2)]
            + sources + ['-o', str(workdir / 'odr.out')],
            capture_output=True, text=True)
        if odr.returncode == 0:
            print('  %spass%s     %-16s %sno function bodies in the header%s'
                  % (GREEN, RESET, spec.name + '.hpp', DIM, RESET))
        else:
            failures += 1
            print('  %sFAIL%s     %-16s ODR: %s' % (
                RED, RESET, spec.name + '.hpp',
                odr.stderr.strip().split('\n')[0]))

    # 5. the forbidden keywords never appear in generated CODE. Comments are
    # stripped first: the generated operator<< carries a note saying it is
    # deliberately not a friend, and a checker that cannot tell an explanation
    # from a violation trains you to stop writing explanations.
    total += 1
    banned = []
    for path in list(workdir.glob('*.hpp')) + list(workdir.glob('*.cpp')):
        text = re.sub(r'/\*.*?\*/', '', path.read_text(), flags=re.DOTALL)
        text = re.sub(r'//[^\n]*', '', text)
        for word in ('using namespace', 'friend', 'nullptr', 'auto ',
                     'malloc(', 'printf('):
            if re.search(r'(^|[^_a-zA-Z])%s' % re.escape(word), text):
                banned.append('%s: %s' % (path.name, word))
    if banned:
        failures += 1
        print('  %sFAIL%s     %-16s %s' % (RED, RESET, 'forbidden',
                                           '; '.join(banned)))
    else:
        print('  %spass%s     %-16s %sno using namespace, friend or C'
              ' functions%s' % (GREEN, RESET, 'forbidden', DIM, RESET))

    print()
    if failures:
        print('%s%s%d of %d checks failed%s  (%s kept for inspection)'
              % (RED, BOLD, failures, total, RESET, workdir))
        return 1
    print('%s%sAll %d checks passed.%s' % (GREEN, BOLD, total, RESET))
    shutil.rmtree(workdir, ignore_errors=True)
    return 0


# ── entry point ──────────────────────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(
        description='Generate 42-style C++98 classes with the boilerplate '
                    'already written.',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='kinds:\n' + '\n'.join(
            '  %-12s %s' % (k, v) for k, v in KINDS.items()))
    parser.add_argument('name', nargs='?', help='class name')
    parser.add_argument('--kind', choices=sorted(KINDS), default=None)
    parser.add_argument('--base', help='base class (implies --kind derived)')
    parser.add_argument('--base-header', help='header declaring the base')
    parser.add_argument('--attr', action='append', default=[],
                        metavar='"std::string name"',
                        help='data member; repeatable')
    parser.add_argument('--method', action='append', default=[],
                        metavar='"void speak() const"', help='repeatable')
    parser.add_argument('--pure', action='append', default=[],
                        metavar='"void makeSound() const"',
                        help='pure virtual member; repeatable')
    parser.add_argument('--exception', action='append', default=[],
                        metavar='GradeTooHighException', help='repeatable')
    parser.add_argument('--template-param', action='append', default=[],
                        metavar='"typename T"')
    parser.add_argument('--namespace', help='wrap the class in a namespace')
    parser.add_argument('--ostream', action='store_true',
                        help='free operator<<')
    parser.add_argument('--protected-pure', action='store_true',
                        help='pure virtuals go under protected: (the AForm '
                             'template-method shape) instead of public')
    parser.add_argument('--no-setters', action='store_true')
    parser.add_argument('--no-value-ctor', action='store_true')
    parser.add_argument('--dir', default='.', help='where to write')
    parser.add_argument('--with-test', action='store_true',
                        help='also write tests/<name>_test.cpp')
    parser.add_argument('--update-makefile', action='store_true',
                        help="add the .cpp to the Makefile's SRC_LIST")
    parser.add_argument('--force', action='store_true', help='overwrite')
    parser.add_argument('--dry-run', action='store_true',
                        help='print instead of writing')
    parser.add_argument('--login', help='override the 42 login in the header')
    parser.add_argument('--email', help='override the email')
    parser.add_argument('--self-test', action='store_true',
                        help='generate every kind and compile it')
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    # Pointed at the destination, not the shell's cwd: the whole point is to
    # copy the By: line from the files that will sit next to this one.
    login, email = identity(args.dir)
    login = args.login or login
    email = args.email or email

    explicit = any([args.kind, args.base, args.attr, args.method, args.pure,
                    args.exception, args.ostream])

    if args.name and explicit:
        kind = args.kind or ('derived' if args.base else 'base')
        methods = [Method(m) for m in args.method]
        methods += [Method(p, force_virtual=True, force_pure=True)
                    for p in args.pure]
        spec = ClassSpec(
            name=args.name, kind=kind, base=args.base,
            base_header=args.base_header,
            attrs=[Attribute(a) for a in args.attr],
            methods=methods, exceptions=args.exception, ostream=args.ostream,
            value_ctor=not args.no_value_ctor, setters=not args.no_setters,
            template_params=args.template_param or None,
            namespace=args.namespace, protected_pure=args.protected_pure)
        with_test = args.with_test
        touch_makefile = args.update_makefile
    else:
        try:
            spec, with_test, touch_makefile = wizard(args.name, args.dir)
        except KeyboardInterrupt:
            print('\n%saborted%s' % (YELLOW, RESET))
            return 1
        with_test = with_test or args.with_test
        touch_makefile = touch_makefile or args.update_makefile

    print()
    written = generate(spec, args.dir, login, email, force=args.force,
                       dry_run=args.dry_run, with_test=with_test,
                       touch_makefile=touch_makefile)
    if not args.dry_run and written:
        print('\n%s%s%s written.%s Next: %smake re%s'
              % (BOLD, GREEN, len(written), RESET, CYAN, RESET))
    return 0


if __name__ == '__main__':
    sys.exit(main())
