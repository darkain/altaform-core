<?php

$afconfig('password', [
	'length'	=> 8,		//Minimum length
]);


assert(afUser::validatePassword('test') === 'Password must be at least 8 characters long');
assert(afUser::validatePassword('test1324') === 'Password must be at least 8 characters long');



echo 'GOOD!!';
