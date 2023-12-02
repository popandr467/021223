import requests,schedule,json
from time import sleep
from pywebio.output import put_text,put_table,use_scope,span,put_html,put_datatable,datatable_update
from pywebio import start_server

fds1=[
    ['Канал 1','Программа'],
    ['Канал 1','Процент выполнения программы']
]
fds2=[['Канал 1','Статус канала']]

def unpack(data,fields):
    lst=[]
    for i in fields:
        if isinstance(i,(list,tuple)):
            dt=data[i[0]]
            nm=i[-1]
            for j in i[1:]:dt=dt[j]
        else:dt,nm=data[i],i
        lst.append({'поле':nm,"значение":dt})
    return lst


def out(tbl):datatable_update('123321',tbl)
def parse(lst):return dict(lst)
def work():out(prepare())
def prepare():return unpack(req(3),fds1)+unpack(req(5),fds2)
def req(n:int):
    s=requests.get(f'https://cnc.kovalev.team/get/{n}').content
    ev=parse(eval(s)['data'])
    #return ev
    ev={i:parse(j) for i,j in ev.items()}
    return ev
    
def start():
    put_text('Программа')
    with use_scope('table',clear=1) as table:put_datatable(prepare(),instance_id='123321')
    while True:
        schedule.run_pending()
        sleep(.05)

schedule.every().second.do(work)
start_server(start, port=8080, debug=True)
