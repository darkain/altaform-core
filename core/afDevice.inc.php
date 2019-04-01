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


class afDevice {

	////////////////////////////////////////////////////////////////////////////
	// MAIN CONSTRUCTOR - INITIALIZE USER AGENT INFO
	////////////////////////////////////////////////////////////////////////////
	public function __construct() {
		static::device();
	}




	////////////////////////////////////////////////////////////////////////////
	// INVOKING THIS CLASS RETURNS THE USER AGENT STRING
	////////////////////////////////////////////////////////////////////////////
	public function __invoke() {
		return static::device();
	}




	////////////////////////////////////////////////////////////////////////////
	// CONVERTING THIS CLASS TO A STRING RETURNS THE USER AGENT STRING
	////////////////////////////////////////////////////////////////////////////
	public function __toString() {
		return static::device();
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE USER AGENT STRING
	////////////////////////////////////////////////////////////////////////////
	public static function agent($agent=false) {
		if ($agent !== false) {
			self::$agent = afString::doublespace(
				strtolower((string)$agent)
			);
		}

		if (empty(self::$device)) static::device();

		return self::$agent;
	}




	////////////////////////////////////////////////////////////////////////////
	// MANUALLY SET A NEW USER AGENT STRING
	////////////////////////////////////////////////////////////////////////////
	public static function set($device) {
		self::$device = $device;
	}




	////////////////////////////////////////////////////////////////////////////
	// FORCE AUTOMATIC REDETECTION OF USER AGENT STRING
	////////////////////////////////////////////////////////////////////////////
	public static function redetect() {
		self::$device = NULL;
		return static::device();
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS NEW ENOUGH TO SUPPORT ENHANCED HTTPS SECURITY
	////////////////////////////////////////////////////////////////////////////
	public static function secure() {
		if (static::is('windows nt 5.')) return false;
		foreach (self::$secure as $key => $agent) {
			if (static::is($agent)) {
				return is_string($key) ? static::is($key) : true;
			}
		}
		return false;
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS A DESKTOP BROWSER
	////////////////////////////////////////////////////////////////////////////
	public static function desktop() {
		return (static::device() === 'desktop' );
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS A MOBILE DEVICE, SUCH AS A SMART PHONE
	////////////////////////////////////////////////////////////////////////////
	public static function mobile() {
		return (static::device() === 'mobile');
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS A MOBILE TABLET DEVICE
	////////////////////////////////////////////////////////////////////////////
	public static function tablet() {
		return (static::device() === 'tablet');
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS A SMART TV
	////////////////////////////////////////////////////////////////////////////
	public static function tv() {
		return (static::device() === 'tv');
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT MATCHES A PARTICULAR STRING
	////////////////////////////////////////////////////////////////////////////
	public static function is($type) {
		return strpos(self::agent(), $type) !== false;
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS LEGACY MICROSOFT INTERNET EXPLORER BROWSER
	////////////////////////////////////////////////////////////////////////////
	public static function trident() {
		return	static::is('trident/')	||
				static::is('bingpreview/');
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS MODERN MICROSOFT EDGE BROWSER
	////////////////////////////////////////////////////////////////////////////
	public static function edge() {
		return static::is('edge/');
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS WEBKIT BASED (CHROME, OPERA, SAFARI, VIVALDI)
	////////////////////////////////////////////////////////////////////////////
	public static function webkit() {
		return static::is('applewebkit/') && !self::edge();
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS MAINLINE FIREFOX OR DEBIAN'S ICEWEASEL PORT
	////////////////////////////////////////////////////////////////////////////
	public static function firefox() {
		return	static::is('firefox/')	||
				static::is('iceweasel/');
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS LEGACY KONQUEROR BROWSER
	////////////////////////////////////////////////////////////////////////////
	public static function konqueror() {
		return static::is('konqueror/');
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER AGENT IS IDENTIFYING ITSELF AS A BOT
	////////////////////////////////////////////////////////////////////////////
	public static function bot() {
		return	static::is('spider/')	||
				static::is('bot/')		||
				static::is('baidu/')	||
				static::is('cawl/')		||
				static::is('slurp/');
	}




	////////////////////////////////////////////////////////////////////////////
	// MAIN USER AGENT PROCESSING AND DETECTION METHOD
	// categorizr function
	////////////////////////////////////////////////////////////////////////////
	static function device() {
		global $get;

		if (self::$device !== NULL) return self::$device;

		//Set User Agent = self::$agent
		if (empty(self::$agent)) {
			self::$device = 'unknown';
			self::agent(\af\cli() ? 'cli' : $get->server('HTTP_USER_AGENT'));
		}

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

		} else if (\af\cli()) {
			self::$device = 'desktop';

		// Otherwise assume it is a Mobile Device
		} else {
			self::$device = 'mobile';
		}

		// Sets self::$device = to what category UA falls into
		return self::$device;

	}// End categorizr function




	////////////////////////////////////////////////////////////////////////////
	// DISPLAY DEVICE INFO ON VAR_DUMP
	////////////////////////////////////////////////////////////////////////////
	public function __debugInfo() {
		return [
			'device'	=> self::$device,
			'agent'		=> self::$agent,
		];
	}




	////////////////////////////////////////////////////////////////////////////
	// LOCAL VARIABLES
	////////////////////////////////////////////////////////////////////////////

	/** @var ?string */
	public static $device	= NULL;

	/** @var string */
	public static $agent	= '';

	/** @var string[] */
	public static $secure	= [
		'applewebkit/',	'konqueror/',
		'firefox/',		'iceweasel/',
		'rv:11.' =>		'trident/',
	];

}
