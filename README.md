## mod_ssl_stub - sets https variable for Apache reverse proxy mode

### Summary

Sets `HTTPS`, and `SSL_TLS_SNI` to the values provided by an upstream proxy.

### Compile Debian/Ubuntu Package and Install

    sudo apt-get install build-essential apache2-threaded-dev
    make
    make install

### Compile and Install for RedHat/CentOS

    yum install httpd-devel
    make
    make install

## Authors:

* Thomas Eibner <thomas@stderr.net>
* Geoffrey McRae <gnif@xbmc.org>
* Proxigence Inc. <support@proxigence.com>
* s3rj1k <evasive.gyron@gmail.com>

## License and distribution

This software is licensed under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).

Code was extracted from [GitHub](http://github.com/gnif/mod_rpaf)
