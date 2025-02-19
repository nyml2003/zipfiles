#!/usr/bin/env python3
include_path = '/app/include/mp/apis'
src_path = '/app/src/server/socket/apis'

code = []

# Example: get file list
api = input('api name: ')

define_name = f'ZIPFILES_MP_APIS_{api.upper().replace(" ", "_")}_H'
api_name = ''.join([i.capitalize() for i in api.split(' ')])

code.append(f'#ifndef {define_name}')
code.append(f'#define {define_name}')
code.append('')
code.append('namespace zipfiles {')
code.append('')
code.append('namespace request {')
code.append(f'struct {api_name} {{')
code.append('  // TODO: add request fields')
code.append('};')
code.append('}  // namespace request')
code.append('')
code.append('namespace response {')
code.append('')
code.append(f'struct {api_name} {{')
code.append('  // TODO: add response fields')
code.append('};')
code.append('}  // namespace response')
code.append('')
code.append('} // namespace zipfiles')
code.append('')
code.append(f'#endif  // !{define_name}')

# 写入到文件include_path/{apiName}.h
with open(f'{include_path}/{api_name}.h', 'w') as f:
    f.write('\n'.join(code))
    
code = []

code.append('#include "mp/Request.h"')
code.append('#include "mp/Response.h"')
code.append('#include "server/socket/api.h"')
code.append('#include "server/socket/socket.h"')
code.append('')
code.append(f'namespace zipfiles::server::api {{')
code.append('')
code.append(f'void {api_name}(int client_fd, const Req& req) {{')
code.append('  // TODO: implement')
code.append('}')
code.append('')
code.append('}  // namespace zipfiles::server::api')


# 写入到文件src_path/{apiName}.cpp
with open(f'{src_path}/{api_name}.cpp', 'w') as f:
    f.write('\n'.join(code))