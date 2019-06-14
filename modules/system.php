<?php


namespace af\system;




////////////////////////////////////////////////////////////////////////////////
// GET THE MAIN OS PATH
////////////////////////////////////////////////////////////////////////////////
function path() {
	switch (true) {
		case bsd():		return '/usr/local/';
		case windows():	return 'c:/';
	}
	return '/';
}




////////////////////////////////////////////////////////////////////////////////
// GET THE FULL OPERATING SYSTEM STRING
// http://php.net/manual/en/function.php-uname.php
////////////////////////////////////////////////////////////////////////////////
function name() {
	return php_uname('a');
}




////////////////////////////////////////////////////////////////////////////////
// GET THE OPERATING SYSTEM NAME
// http://php.net/manual/en/function.php-uname.php
////////////////////////////////////////////////////////////////////////////////
function os() {
	return php_uname('s');
}




////////////////////////////////////////////////////////////////////////////////
// GET THE OPERATING SYSTEM RELEASE
// http://php.net/manual/en/function.php-uname.php
////////////////////////////////////////////////////////////////////////////////
function release() {
	return php_uname('r');
}




////////////////////////////////////////////////////////////////////////////////
// GET THE OPERATING SYSTEM VERSION
// http://php.net/manual/en/function.php-uname.php
////////////////////////////////////////////////////////////////////////////////
function version() {
	return php_uname('v');
}




////////////////////////////////////////////////////////////////////////////////
// GET THE OPERATING SYSTEM ARCHITECTURE (X86, X86_64, ARM, ARM64)
// http://php.net/manual/en/function.php-uname.php
////////////////////////////////////////////////////////////////////////////////
function arch() {
	return php_uname('m');
}




////////////////////////////////////////////////////////////////////////////////
// GET THE OS USED TO BUILD THIS PHP BINARY
// http://php.net/manual/en/reserved.constants.php
////////////////////////////////////////////////////////////////////////////////
function builder() {
	return PHP_OS;
}




////////////////////////////////////////////////////////////////////////////////
// BOOLEAN CHECK IF WE'RE ON WINDOWS
////////////////////////////////////////////////////////////////////////////////
function windows() {
	return stripos('WINDOWS', os()) !== false;
}




////////////////////////////////////////////////////////////////////////////////
// BOOLEAN CHECK IF WE'RE ON (GNU/)LINUX
////////////////////////////////////////////////////////////////////////////////
function linux() {
	return stripos('LINUX', os()) !== false;
}




////////////////////////////////////////////////////////////////////////////////
// BOOLEAN CHECK IF WE'RE ON FREEBSD
////////////////////////////////////////////////////////////////////////////////
function freebsd() {
	return stripos('FREEBSD', os()) !== false;
}




////////////////////////////////////////////////////////////////////////////////
// BOOLEAN CHECK IF WE'RE ON BSD (ANY FLAVOR)
////////////////////////////////////////////////////////////////////////////////
function bsd() {
	return stripos('BSD', os()) !== false;
}




////////////////////////////////////////////////////////////////////////////////
// BOOLEAN CHECK IF WE'RE ON MAC-OS
////////////////////////////////////////////////////////////////////////////////
function mac() {
	return stripos('DARWIN', os()) !== false;
}




////////////////////////////////////////////////////////////////////////////////
// BOOLEAN CHECK IF WE'RE ON SUN-OS / SOLARIS / SMARTOS
////////////////////////////////////////////////////////////////////////////////
function sun() {
	return stripos('SUNOS', os()) !== false;
}