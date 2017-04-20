<?php

$afconfig('password', [
	'length'	=> 8,		//Minimum length
]);


assert(af_user::validatePassword('test') === 'Password must be at least 8 characters long');
assert(af_user::validatePassword('test1324') === 'Password must be at least 8 characters long');



echo 'GOOD!!';
