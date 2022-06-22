# Altaform
[![Build Status](https://travis-ci.com/darkain/altaform-core.svg?branch=master)](https://app.travis-ci.com/github/darkain/altaform-core)




## About
Altaform is a small, simple, lightweight micro-framework for PHP web
applications. Altaform handles HTML templates, database connectivity,
$\_GET/$\_POST reading, URL routing, error logging, and more automatically.
Altaform strives to be a zero-boilerplate framework focusing entirely
on allowing developers to jump in to create simplistic and clean PHP
code without worrying about what classes need to be inherited or what
methods need overwritten. Altaform allows for traditional minimalistic
procedural PHP programming while still having many of the same benefits
of a modern PHP framework.




## License
This software library is licensed under the BSD 2-clause license, and may be
freely used in any project (commercial, freelance, hobby, or otherwise) which
is compatible with this license. See
[LICENSE](https://github.com/darkain/altaform/blob/master/LICENSE)
for more details.




## Compatibility
Altaform is actively tested and used in production on PHP 7.2 and HHVM 3.24.

Altaform should work as far back as PHP 5.4, but this is no longer actively
used in production. Altaform heavily relies upon new features introduced in
PHP 5.4, therefor will NOT work on any version of PHP prior to 5.4.

Altaform may work on versions of HHVM prior to 3.12, but it is highly
discouraged. During Altaform development, a countless number of bugs have been
discovered and reported in the HHVM interpreter that directly affected this
project. As such, Altaform is not guaranteed to function properly on earlier
versions of HHVM prior to 3.24.




## Modules
Path | Library | Usage
-----|---------|------
[\_altaform](https://github.com/darkain/altaform-core) | Altaform Core | URL router, user access
[\_getvar](https://github.com/darkain/getvar) | GetVar | Handler for $\_GET/$\_POST data
[\_pudl](https://github.com/darkain/pudl) | PHP Universal Database Library (PUDL) | DB connection and SQL query generator/processor
[\_tbx](https://github.com/darkain/TinyButXtreme) | TinyButExtreme (TBX) | HTML5 template processor




## Global Variables
Name | Library | Usage | Documentation
-----|---------|-------|--------------
$af | Altaform Core & TBX | Instance of the [altaform](https://github.com/darkain/altaform-core/blob/master/core/afCore.inc.php) class, inherits [tbx](https://github.com/darkain/TinyButXtreme/blob/master/tbx_class.inc.php) class | [Documentation](https://github.com/darkain/altaform-core/blob/master/README.md)
$afurl | Altaform Core | Instance of the [afUrl](https://github.com/darkain/altaform-core/blob/master/core/afUrl.inc.php) class
$router | Altaform Core | Instance of the [afRouter](https://github.com/darkain/altaform-core/blob/master/router/router.php) class
$user | Altaform Core | Instance of the [afUser](https://github.com/darkain/altaform-core/blob/master/core/afUser.inc.php) class
$get | GetVar | Instance of the [getvar](https://github.com/darkain/getvar/blob/master/getvar.inc.php) class | [Documentation](https://github.com/darkain/getvar/blob/master/README.md)
$db | PUDL | Instance of one of the [pudl](https://github.com/darkain/pudl/blob/master/pudl.php) classes (depends on database type) | [Documentation](https://github.com/darkain/pudl/blob/master/README.md)




## Getting Started
These instructions assume a basic LAMP stack (Linux + Apache + MySQL + PHP),
however in production we actively have variations of each of these in use.
Altaform has been tested on Linux, FreeBSD and SmartOS for the operating system,
Apache, Nginx, and Lighttpd for the web server, MySQL, MariaDB, and SQLite for
the database, and PHP 5.x, PHP 7, and HHVM for the interpreter. These examples
listed below have been tested and work with the
[TurnKey Linux LAMP appliance](https://www.turnkeylinux.org/lampstack)
running under VMWare vSphere, but should be the same on any LAMP stack setup
with **mod_rewrite** enabled and the document root in **/var/www**.

By default, TurnKey LAMP has some problematic modules enabled, and required
modules disabled. Our first task is to correct these issues. Negotiation causes
URL rewriting conflicts, and rewrite is needed to support a single PHP entry
point.
```Bash
cd /etc/apache2/mods-enabled/
rm negotiation.*
ln -s ../mods-available/rewrite.load
service apache2 restart
```

By default, TurnKey LAMP does not include root certificates to validate HTTPS
connections made by git. These need to be added to the system.
```Bash
apt-get install ca-certificates
```

By default, TurnKey LAMP installs a bunch of cruft into /var/www, so we need to
empty the folder.
```Bash
cd /var/www
rm -R *
```

Next we need to clone the Altaform code from GitHub.
```Bash
git clone https://github.com/darkain/altaform.git .
```

Now we need to clone all of the sub-modules from GitHub.
```Bash
sh _scripts/submodules.sh
```
