<?php


////////////////////////////////////////////////////////////////////////////////
//VERIFY TABLES EXIST AND ARE CURRENTLY EMPTY
////////////////////////////////////////////////////////////////////////////////
assertStatus(500,
	$db->tableExists('pudl_user'),
	'USER TABLE DOES NOT EXIST'
);


assertStatus(500,
	$db->tableExists('pudl_user_auth'),
	'AUTH TABLE DOES NOT EXIST'
);


assertStatus(500,
	$db->row('pudl_user') === false,
	'USER TABLE IS ALREADY POPULATED'
);


assertStatus(500,
	$db->row('pudl_user_auth') === false,
	'AUTH TABLE IS ALREADY POPULATED'
);




////////////////////////////////////////////////////////////////////////////////
//ALLOW FOR INSERTING '0' VALUE INTO AN AUTO INCREMENT COLUMN
////////////////////////////////////////////////////////////////////////////////
$db->set('sql_mode', 'NO_AUTO_VALUE_ON_ZERO');




////////////////////////////////////////////////////////////////////////////////
//TRANSACTIONS
////////////////////////////////////////////////////////////////////////////////
$db->begin();




////////////////////////////////////////////////////////////////////////////////
//CREATE INITIAL ANONYMOUS USER ACCOUNT
////////////////////////////////////////////////////////////////////////////////
$anonymous = afUser::create($db, [
	'user_id'			=> 0,
	'user_permission'	=> 'guest',
	'user_name'			=> 'anonymous',
]);




////////////////////////////////////////////////////////////////////////////////
//CREATE INITIAL ADMIN USER ACCOUNT
////////////////////////////////////////////////////////////////////////////////
$user = afUser::create($db, [
	'user_id'			=> 1,
	'user_permission'	=> 'admin',
	'user_name'			=> 'admin',
	'user_url'			=> 'admin',
]);




////////////////////////////////////////////////////////////////////////////////
//GENERATE A RANDOM PASSWORD FOR ADMIN USER
//TODO: SET PASSWORD STRENGTH
////////////////////////////////////////////////////////////////////////////////
$password = afString::password();
$user->setPassword('admin', $password);




////////////////////////////////////////////////////////////////////////////////
//TRANSACTIONS
////////////////////////////////////////////////////////////////////////////////
$db->commit();




////////////////////////////////////////////////////////////////////////////////
//RENDER ALL THE THINGS!!
////////////////////////////////////////////////////////////////////////////////
$af	->header()
		->load('index.tpl')
			->field('password', $password)
		->render()
	->footer();
