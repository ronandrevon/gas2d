import pandas as pd
from subprocess import check_output

prof_in ="data/prof.txt"
prof_out="data/prof.out"

cmd="\
sed -E 's/([0-9.]+) {1,}/\\1;/g' %s \
| tail --lines=+6 \
| sed 's/^ *//' \
> %s\
" %(prof_in,prof_out)
print(cmd)
check_output(cmd,shell=True).decode()
# print(buf)

df = pd.read_csv(prof_out,sep=";",
    names=['time%','cum_sec','sec','calls','ms/calls','total_ms','name'],
    )
