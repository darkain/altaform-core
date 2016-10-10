<?php

assert401(
	$auth = $db->rowId('pudl_user_auth', 'auth_account', $get)
);

assert401(
	password_verify(
		$get->password('auth_password'),
		$auth['auth_password']
	)
);

$af->authenticate($auth);

if (!empty($get('redirect'))) {
	$afurl->redirect($get('redirect'));
} else {
	$afurl->redirect([]);
}
