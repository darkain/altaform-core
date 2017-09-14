<?php
/**
* Modification by Darkain Multimedia for use in Altaform
**/


/**
* Categorizr Version 1.1
* http://www.brettjankord.com/2012/01/16/categorizr-a-modern-device-detection-script/
* Written by Brett Jankord - Copyright © 2011
* Thanks to Josh Eisma for helping with code review
*
* Big thanks to Rob Manson and http://mob-labs.com for their work on
* the Not-Device Detection strategy:
* http://smartmobtoolkit.wordpress.com/2009/01/26/not-device-detection-javascript-perl-and-php-code/
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
* You should have received a copy of the GNU General Public License
* and GNU Lesser General Public License
* along with this program. If not, see http://www.gnu.org/licenses/.
**/


class afdevice {
	static $device = false;


	public function __construct() { self::device(); }

	public function __invoke() { return self::device(); }

	public function __toString() { return self::device(); }


	static function device() {
		global $get;

		if (self::$device !== false) return self::$device;

		//Set User Agent = self::$agent
		self::$agent = strtolower($get->server('HTTP_USER_AGENT', ''));

		// No user agent
		if (empty(self::$agent)) {
			self::$device = 'desktop';

		// Check if user agent is a smart TV - http://goo.gl/FocDk
		} else if ((preg_match('/AndroidTV|GoogleTV|SmartTV|Internet.TV|NetCast|NETTV|AppleTV|boxee|Kylo|Roku|DLNADOC|CE\-HTML/i', self::$agent))) {
			self::$device = 'tv';

		// Check if user agent is a TV Based Gaming Console
		} else if ((preg_match('/Xbox|PLAYSTATION|Wii|Nintendo/i', self::$agent))) {
			self::$device = 'tv';

		// Check if user agent is a Tablet
		} else if((preg_match('/iP(a|ro)d/i', self::$agent)) || (preg_match('/tablet/i', self::$agent)) && (!preg_match('/RX-34/i', self::$agent)) || (preg_match('/FOLIO/i', self::$agent))) {
			self::$device = 'tablet';

		// Check if user agent is an Android Tablet
		} else if ((preg_match('/Linux/i', self::$agent)) && (preg_match('/Android/i', self::$agent)) && (!preg_match('/Fennec|mobi|HTC.Magic|HTCX06HT|Nexus.One|SC-02B|fone.945/i', self::$agent))) {
			self::$device = 'tablet';

		// Check if user agent is a Kindle or Kindle Fire
		} else if ((preg_match('/Kindle/i', self::$agent)) || (preg_match('/Mac.OS/i', self::$agent)) && (preg_match('/Silk/i', self::$agent))) {
			self::$device = 'tablet';

		// Check if user agent is a pre Android 3.0 Tablet
		} else if ((preg_match('/GT-P10|SC-01C|SHW-M180S|SGH-T849|SCH-I800|SHW-M180L|SPH-P100|SGH-I987|zt180|HTC(.Flyer|\_Flyer)|Sprint.ATP51|ViewPad7|pandigital(sprnova|nova)|Ideos.S7|Dell.Streak.7|Advent.Vega|A101IT|A70BHT|MID7015|Next2|nook/i', self::$agent)) || (preg_match('/MB511/i', self::$agent)) && (preg_match('/RUTEM/i', self::$agent))) {
			self::$device = 'tablet';

		// Check if user agent is unique Mobile User Agent
		} else if ((preg_match('/BOLT|Fennec|Iris|Maemo|Minimo|Mobi|mowser|NetFront|Novarra|Prism|RX-34|Skyfire|Tear|XV6875|XV6975|Google.Wireless.Transcoder/i', self::$agent))) {
			self::$device = 'mobile';

		// Check if user agent is an odd Opera User Agent - http://goo.gl/nK90K
		} else if ((preg_match('/Opera/i', self::$agent)) && (preg_match('/Windows.NT.5/i', self::$agent)) && (preg_match('/HTC|Xda|Mini|Vario|SAMSUNG\-GT\-i8000|SAMSUNG\-SGH\-i9/i', self::$agent))) {
			self::$device = 'mobile';

		// Check if user agent is Windows Desktop
		} else if ((preg_match('/Windows.(NT|XP|ME|9)/i', self::$agent)) && (!preg_match('/Phone/i', self::$agent)) || (preg_match('/Win(9|.9|NT)/i', self::$agent))) {
			self::$device = 'desktop';

		// Check if agent is Mac Desktop
		} else if ((preg_match('/Macintosh|PowerPC|Mac.OS/i', self::$agent)) && (!preg_match('/Silk/i', self::$agent))) {
			self::$device = 'desktop';

		// Check if user agent is a Linux Desktop
		} else if ((preg_match('/Linux/i', self::$agent)) && (preg_match('/X11/i', self::$agent))) {
			self::$device = 'desktop';

		// Check if user agent is a Solaris, SunOS, BSD Desktop
		} else if ((preg_match('/Solaris|SunOS|BSD/i', self::$agent))) {
			self::$device = 'desktop';

		// Check if user agent is a Desktop BOT/Crawler/Spider
		} else if ((preg_match('/Bot|Crawler|Spider|Yahoo|ia_archiver|Covario-IDS|findlinks|DataparkSearch|larbin|Mediapartners-Google|NG-Search|Snappy|Teoma|Jeeves|TinEye|Validator/i', self::$agent)) && (!preg_match('/Mobile/i', self::$agent))) {
			self::$device = 'desktop';

		// Otherwise assume it is a Mobile Device
		} else {
			self::$device = 'mobile';
		}

		// Sets self::$device = to what category UA falls into
		return self::$device;

	}// End categorizr function



	public static function is_secure() {
		if (self::is('windows nt 5.')) return false;
		foreach (self::$secure as $key => $agent) {
			if (self::is($agent)) {
				return is_string($key) ? self::is($key) : true;
			}
		}
		return false;
	}



	public static function set($device)	{ self::$device = $device; }
	public static function desktop()	{ return (self::device() === 'desktop'	); }
	public static function tablet()		{ return (self::device() === 'tablet'	); }
	public static function tv()			{ return (self::device() === 'tv'		); }
	public static function mobile()		{ return (self::device() === 'mobile'	); }
	public static function redetect()	{ self::$device = false; return self::device(); }

	public static function agent()		{ return self::$agent; }
	public static function is($type)	{ return strpos(self::$agent, $type)!==false; }
	public static function trident()	{ return self::is('trident/')		||  self::is('bingpreview/'); }
	public static function edge()		{ return self::is('edge/'); }
	public static function webkit()		{ return self::is('applewebkit/')	||  !self::edge(); }
	public static function konqueror()	{ return self::is('konqueror/'); }
	public static function firefox()	{ return self::is('firefox/')		||  self::is('iceweasel/'); }
	public static function bot()		{ return self::is('spider/')		||  self::is('bot/')	||  self::is('baidu/')	||  self::is('cawl/')	||  self::is('slurp/'); }

	public static $agent	= '';

	public static $secure	= [
		'applewebkit/',	'konqueror/',
		'firefox/',		'iceweasel/',
		'bot/',			'spider/',
		'rv:11.' =>		'trident/',
	];

}

afdevice::device();
