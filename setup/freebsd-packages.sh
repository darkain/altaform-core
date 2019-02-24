#!/bin/sh

# INSTALL FREEBSD PHP PACKAGES
pkg install -y php73 php73-curl php73-exif php73-sodium php73-zip php73-zlib php73-mysqli php73-pecl-imagick php73-pecl-redis php73-sysvmsg php73-sysvsem php73-sysvshm php73-sockets php73-simplexml php73-extensions

# ENABLE PHP-FPM ON ALL IPv4 ADDRESSES
sed -i .altaform 's/127.0.0.1:9000/0.0.0.0:9000/' /usr/local/etc/php-fpm.d/www.conf

# ENABLE THE PHP-FPM DAEMON
LINE='php_fpm_enable="YES"'
FILE=/etc/rc.conf.local
grep -qF -- "$LINE" "$FILE" || echo "$LINE" >> "$FILE"

# START THE PHP-FPM DAEMON
service php-fpm start
