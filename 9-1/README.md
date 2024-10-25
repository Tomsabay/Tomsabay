# 9.1.1 Kvstore

```
# git clone http://gitlab.0voice.com/2310_vip/9.1.1-kvstore.git
# git submodule update --init 
```

```
# gcc -o kvstore kvstore.c ntyco_entry.c epoll_entry.c kvstore_array.c -I ./NtyCo/core/ -L ./NtyCo/ -lntyco
```