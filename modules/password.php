<?php


namespace af;



////////////////////////////////////////////////////////////////////////////////
// ALLOWED CHARACTERS IN SYSTEM GENERATED PASSWORDS
////////////////////////////////////////////////////////////////////////////////
const password_allowed =
	'01234abcdefghijklmnopqrstuvwxyz-^/(#_56789ABCDEFGHIJKLMNOPQRSTUVWXYZ+)\\$=';


function password($length=16) {
	$allowed	= password_allowed;
	$password	= '';
	$characters	= strlen($allowed)-1;
	for ($i=0; $i<$length; $i++) {
		$password .= $allowed[random_int(0, $characters)];
	}
	return $password;
}
