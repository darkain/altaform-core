<?php

require_once('af_ip.php.inc');


/*//////////////////////////////////////////////////////////////////////////////
\ TO USE THIS CLASS, CALL
/
\ af_audit::something($type, $id, $old [optional], $new [optional])
/
\ METHOD NAME something CAN BE ANYTHING, AND IS USED TO LOG THE TYPE OF ACTION
/ THAT IS TAKING PLACE. FOR INSTANCE, something CAN BE create TO DENOTE THE
\ CREATION OF A NEW OBJECT.
/
\ $type:	OBJECT TYPE, AS DEFINED BY ALTAFORM GLOBAL TYPES
/ $id:		OBJECT ID, AS DEFINED BY APPLICATION / DATABASE ID
\ $old:		PREVIOUS DATA ASSOCIATED WITH THE OBJECT [optional]
/ $new:		NEW DATA ASSOCIATED WITH THE OBJECT [optional]
\/////////////////////////////////////////////////////////////////////////////*/




class af_audit {


	public static function __callStatic($action, $arguments) {
		global $db, $user;

		//VALIDATE NUMBER OF ARGUMENTS
		if (count($arguments) < 2) {
			throw new afException(
				'Invalid number of arguments for af_audit::' . $action . '()'
			);
		}

		//VALUES WE NEED TO PROCESS
		$address	= af_ip::address();
		$type		= $arguments[0];

		//CONVERT TYPE STRINGS TO THEIR INTEGER VALUE
		if (!is_int($type)  &&  !ctype_digit($type)) {
			$type = altaform::type($type);
		}

		//THROW AN EXCEPTION IF NO TYPE NUMBER AVAILABLE
		if (empty($type)) {
			throw new afException(
				'Invalid object type for af_audit::' . $action . '()'
			);
		}

		//NULL OLD/NEW DATA IF NOT AVAILABLE
		if (!isset($arguments[2])) $arguments[2] = NULL;
		if (!isset($arguments[3])) $arguments[3] = NULL;

		//INSERT AUDIT DATA INTO DATABASE
		return $db->insert('pudl_audit', [
			'user_id'			=> $user->id(),
			'user_ip'			=> empty($address) ? NULL : $address,
			'object_id'			=> (int) $arguments[1],
			'object_type_id'	=> (int) $type,
			'audit_old'			=> $arguments[2],
			'audit_new'			=> $arguments[3],
			'audit_action'		=> $action,
			'audit_timestamp'	=> $db->time(),
		]);
	}


}
