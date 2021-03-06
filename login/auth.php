<?php


////////////////////////////////////////////////////////////////////////////////
// IF WE'RE ALREADY LOGGED IN, JUST REDIRECT PAGE
////////////////////////////////////////////////////////////////////////////////
if ($user->loggedIn()) {
	return require(is_owner(__DIR__.'/redirect.inc.php'));
}




////////////////////////////////////////////////////////////////////////////////
// PULL THE REQUESTED ACCOUNT
////////////////////////////////////////////////////////////////////////////////
\af\affirm(401,
	$auth = $db->row([
		'ua' => 'user_auth',
		'us' => 'user',
	], [
		'auth_account' => $get->auth_account,
		'us.user_id=ua.user_id',
	]),
	'Invalid Email Address or Password'
);




////////////////////////////////////////////////////////////////////////////////
// VERIFY THE PASSWORD
////////////////////////////////////////////////////////////////////////////////
$password = $get->password('auth_password');

\af\affirm(401,
	password_verify($password, $auth['auth_password']),
	'Invalid Email Address or Password'
);




////////////////////////////////////////////////////////////////////////////////
// PASS AUTHENTICATION TO ALTAFORM
////////////////////////////////////////////////////////////////////////////////
$af->authenticate($auth);




////////////////////////////////////////////////////////////////////////////////
// LOAD THE USER PROFILE
////////////////////////////////////////////////////////////////////////////////
$user = new afUser($db, $auth);




////////////////////////////////////////////////////////////////////////////////
// UPDATE THE PASSWORD HASH IF NEEDED
////////////////////////////////////////////////////////////////////////////////
if (!empty($af->config->password['hash'])) {
	$hash = $af->config->password['hash'];
	if (password_needs_rehash($auth['auth_password'], $hash)) {
		$user->setPassword($auth['auth_account'], $password, $hash);
	}
}




////////////////////////////////////////////////////////////////////////////////
// REDIRECT TO BROWSER USER PROFILE
////////////////////////////////////////////////////////////////////////////////
unset($password);
require(is_owner(__DIR__.'/redirect.inc.php'));
