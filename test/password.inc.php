<?php




$password_backup	= $afconfig->password;
$afconfig->password	= [];



$afconfig('password', [
	'length'	=> 8,		//Minimum length
	'upper'		=> true,	//Require upper+lower case letters
	'number'	=> true,	//Require numbers
	'symbol'	=> true,	//Require symbols
]);




afUnit(
	afUser::validatePassword('test'),
	'Password must be at least 8 characters long'
);




afUnit(
	afUser::validatePassword('TestTest'),
	'Password must contain numbers'
);




afUnit(
	afUser::validatePassword('test1324'),
	'Password must contain both upper and lower case letters'
);




afUnit(
	afUser::validatePassword('Test1324'),
	'Password must contain special character symbols'
);




afUnit(afUser::validatePassword('Test132!'));



$afconfig->password	= $password_backup;
