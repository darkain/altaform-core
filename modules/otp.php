<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS FOR HANDLING TWO-FACTOR AUTHENTICATION (2FA)
// THIS CLASS GENERATES AND VALIDATES AUTHENTICATOR ONE-TIME PASSWORDS (OTP)
////////////////////////////////////////////////////////////////////////////////
class otp {


	////////////////////////////////////////////////////////////////////////////
	// CAN SPECIFY A DIFFERENT CODE LENGTH UPON INSTANTIATION
	////////////////////////////////////////////////////////////////////////////
	public function __construct($length=6) {
		$this->length = $length;
	}




	////////////////////////////////////////////////////////////////////////////
	// GENERATE A NEW SECRET
	////////////////////////////////////////////////////////////////////////////
	public function secret($length=16) {
		$secret = '';
		for ($i=0; $i<$length; $i++) {
			$secret .= $this->chars[ random_int(0, strlen($this->chars)-1) ];
		}
		return $secret;
	}




	////////////////////////////////////////////////////////////////////////////
	// GENERATE A CODE WITH A GIVEN SLICE OR CURRENT SLICE IF NONE IS SPECIFIED
	////////////////////////////////////////////////////////////////////////////
	public function code($secret, $slice=false) {
		if (is_object($slice))	$slice = $this->slice($slice);
		if (empty($slice))		$slice = $this->slice();

		$hash = hash_hmac('SHA1',
			"\0\0\0\0" . pack('N*', (int)$slice),
			$this->decode($secret),
			true
		);

		$value = unpack('N',
			substr($hash, ord(substr($hash,-1)) & 0x0F, 4)
		);

		return str_pad(
			(string)(($value[1] & 0x7FFFFFFF) % pow(10, $this->length)),
			$this->length, '0', STR_PAD_LEFT
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE SCANNABLE QRCODE URL TO DISPLAY BACK TO THE END USER
	////////////////////////////////////////////////////////////////////////////
	public function qrcode($name, $secret, $size=200) {
		return 'https://chart.googleapis.com/chart?chs=' .
			$size . 'x' . $size . '&chld=M|0&cht=qr&chl=' .
			urlencode('otpauth://totp/' . $name . '?secret=' . $secret);
	}




	////////////////////////////////////////////////////////////////////////////
	// VERIFY A CODE TO A GIVEN SECRET, WITH OFFSET TOLERANCE FOR SLICES
	////////////////////////////////////////////////////////////////////////////
	public function verify($secret, $code, $offset=1) {
		for ($i = -$offset; $i <= $offset; $i++) {
			$check = $this->code($secret, $this->slice()+$i);
			if ($check === $code) return true;
		}

		return false;
	}




	////////////////////////////////////////////////////////////////////////////
	// BASE32 DECODER, SINCE PHP DOES NOT HAVE THIS BUILT IN FOR SOME REASON
	////////////////////////////////////////////////////////////////////////////
	protected function decode($data) {
		$binary = '';

		for ($i=0; $i<strlen($data); $i++) {
			$ord		 = strpos($this->chars, $data[$i]);
			$binary		.= str_pad(decbin($ord), 5, '0', STR_PAD_LEFT);
		}

		$length = (int) floor(strlen($binary) / 8) * 2;
		$output = str_repeat('_', $length);

		for ($i=0; $i<$length; $i++) {
			$output[$i]	= base_convert(substr($binary, $i*4, 4), 2, 16);
		}

		return hex2bin($output);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE CURRENT TIME SLICE
	////////////////////////////////////////////////////////////////////////////
	protected function slice($source=NULL) {
		$time = $source ? $source->time() : time();
		return floor($time / 30);
	}




	////////////////////////////////////////////////////////////////////////////
	// LOCAL VARIABLES
	////////////////////////////////////////////////////////////////////////////
	protected $length	= 6;
	protected $chars	= 'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567';
}
