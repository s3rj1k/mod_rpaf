APXS=/usr/bin/apxs2

ssl_stub: mod_ssl_stub.so
	@echo make done
	@echo type \"make install\" to install mod_ssl_stub

mod_ssl_stub.so: mod_ssl_stub.c
	$(APXS) -c -n $@ mod_ssl_stub.c

mod_ssl_stub.c:

install: mod_ssl_stub.so
	$(APXS) -i -S LIBEXECDIR=$(DESTDIR)$$($(APXS) -q LIBEXECDIR)/ -n mod_ssl_stub.so mod_ssl_stub.la

clean:
	rm -rf *~ *.o *.so *.lo *.la *.slo *.loT .libs/
