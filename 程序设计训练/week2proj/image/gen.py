import os
rootdir = '.'
qrc = """
<!DOCTYPE RCC><RCC version="1.0">
<qresource>
""".lstrip()

for f in os.listdir(rootdir):
    path = os.path.join(rootdir, f)
    if os.path.isfile(path):
        qrc += '    <file>image/%s</file>\n' % str(f)

qrc += """
</qresource>
</RCC>
""".strip()
print(qrc)
# list = os.listdir(rootdir)
# for i in range(0, len(list)):
