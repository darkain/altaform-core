#!/bin/sh

# INSTALL FREEBSD LIGHTTPD AND PHP PACKAGES
pkg install -y lighttpd php84 php84-curl php84-exif php84-sodium php84-zip \
	php84-zlib php84-mysqli php84-pecl-imagick php84-pecl-redis php84-sysvmsg \
	php84-sysvsem php84-sysvshm php84-sockets php84-simplexml php84-extensions


# ENABLE PHP-FPM ON ALL IPv4 ADDRESSES
sed -i .altaform 's/127.0.0.1:9000/0.0.0.0:9000/' /usr/local/etc/php-fpm.d/www.conf


# ENABLE THE PHP-FPM DAEMON
sysrc 'php_fpm_enable="YES"'

# START THE PHP-FPM DAEMON
service php_fpm start


# ENABLE THE LIGHTTPD DAEMON
sysrc 'lighttpd_enable="YES"'

# START THE LIGHTTPD DAEMON
service lighttpd start
