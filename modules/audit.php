<?php


namespace af;



////////////////////////////////////////////////////////////////////////////////
// TO USE THIS CLASS, CALL
//
// afAudit::something($type, $id, $old [optional], $new [optional])
//
// METHOD NAME something CAN BE ANYTHING, AND IS USED TO LOG THE TYPE OF ACTION
// THAT IS TAKING PLACE. FOR INSTANCE, something CAN BE create TO DENOTE THE
// CREATION OF A NEW OBJECT.
//
// $type:	OBJECT TYPE, AS DEFINED BY ALTAFORM GLOBAL TYPES
// $id:		OBJECT ID, AS DEFINED BY APPLICATION / DATABASE ID
// $old:		PREVIOUS DATA ASSOCIATED WITH THE OBJECT [optional]
// $new:		NEW DATA ASSOCIATED WITH THE OBJECT [optional]
////////////////////////////////////////////////////////////////////////////////




class audit {


	public static function __callStatic($action, $arguments) {

		// VALIDATE NUMBER OF ARGUMENTS
		if (count($arguments) < 4) {
			throw new \afException(
				'Invalid number of arguments for afAudit::' . $action . '()'
			);
		}

		// GET THE DATABASE
		$pudl = $arguments[0];
		if (!($pudl instanceof \pudl)) {
			throw new \afException(
				'"' . gettype($pudl) . '" is not an instance of class "pudl"'
			);
		}

		// GET THE USER
		$user = $arguments[1];
		if (is_object($user)) {
			$user = $user->id();
		}

		// VALUES WE NEED TO PROCESS
		$address	= ip::address();
		$type		= $arguments[2];

		// CONVERT TYPE STRINGS TO THEIR INTEGER VALUE
		if (!is_int($type)  &&  !ctype_digit($type)) {
			$type = \altaform::type($type);
		}

		// THROW AN EXCEPTION IF NO TYPE NUMBER AVAILABLE
		if (empty($type)) {
			throw new \afException(
				'Invalid object type for \af\audit::' . $action . '()'
			);
		}

		// NULL OLD/NEW DATA IF NOT AVAILABLE
		if (!isset($arguments[4])) $arguments[4] = NULL;
		if (!isset($arguments[5])) $arguments[5] = NULL;

		// INSERT AUDIT DATA INTO DATABASE
		return $pudl->insert('pudl_audit', [
			'user_id'			=> $user,
			'user_ip'			=> empty($address) ? NULL : $address,
			'object_id'			=> (int) $arguments[3],
			'object_type_id'	=> (int) $type,
			'audit_old'			=> $arguments[4],
			'audit_new'			=> $arguments[5],
			'audit_action'		=> $action,
			'audit_timestamp'	=> $pudl->time(),
		]);
	}


}
