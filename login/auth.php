<?php


////////////////////////////////////////////////////////////////////////////////
//IF WE'RE ALREADY LOGGED IN, JUST REDIRECT PAGE
////////////////////////////////////////////////////////////////////////////////
if ($user->loggedIn()) {
	return require(af_file_owner(__DIR__.'/redirect.inc.php'));
}




////////////////////////////////////////////////////////////////////////////////
//PULL THE REQUESTED ACCOUNT
////////////////////////////////////////////////////////////////////////////////
assert401(
	$auth = $db->row([
		'ua' => 'pudl_user_auth',
		'us' => 'pudl_user',
	], [
		'auth_account' => $get->auth_account,
		'us.user_id=ua.user_id',
	]),
	'Invalid Email Address or Password'
);




////////////////////////////////////////////////////////////////////////////////
//VERIFY THE PASSWORD
////////////////////////////////////////////////////////////////////////////////
assert401(
	password_verify(
		$get->password('auth_password'),
		$auth['auth_password']
	),
	'Invalid Email Address or Password'
);




////////////////////////////////////////////////////////////////////////////////
//PASS AUTHENTICATION TO ALTAFORM
////////////////////////////////////////////////////////////////////////////////
$af->authenticate($auth);




////////////////////////////////////////////////////////////////////////////////
//REDIRECT THE USER
////////////////////////////////////////////////////////////////////////////////
$user = new afUser($auth);
require(af_file_owner(__DIR__.'/redirect.inc.php'));
