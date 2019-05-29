<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS FOR LOGGING A USER'S ACTIVITY
////////////////////////////////////////////////////////////////////////////////
class activity {


	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	public function __construct(\altaform $af, \afUser $user, \pudl $pudl=NULL) {
		$this->af	= $af;
		$this->user	= $user;
		$this->pudl	= $pudl ? $pudl : $user->pudl();
	}




	////////////////////////////////////////////////////////////////////////////
	// ADD ACTIVITY BY ID
	////////////////////////////////////////////////////////////////////////////
	public function add($id, $type, $verb, $user=NULL) {
		$user = $this->uid($user);
		if (empty($user)) return false;

		return $this->pudl->insert('activity', [
			'activity_timestamp'	=> $this->pudl->time(),
			'user_id'				=> $user,
			'object_id'				=> $id,
			'object_type_id'		=> $this->af->type($type),
			'activity_verb'			=> $verb,
		], [
			'activity_count'		=> \pudl::_increment(1),
			'activity_timestamp'	=> $this->pudl->time(),
		]);
	}




	////////////////////////////////////////////////////////////////////////////
	// ADD ACTIVITY BY FILE HASH
	////////////////////////////////////////////////////////////////////////////
	public function addFile($hash, $type, $verb, $user=NULL) {
		$user = $this->uid($user);
		if (empty($user)) return false;

		return $this->pudl->insert('activity', [
			'activity_timestamp'	=> $this->pudl->time(),
			'user_id'				=> $user,
			'file_hash'				=> $hash,
			'object_type_id'		=> $this->af->type($type),
			'activity_verb'			=> $verb,
		], [
			'activity_count'		=> \pudl::_increment(1),
			'activity_timestamp'	=> $this->pudl->time(),
		]);
	}




	////////////////////////////////////////////////////////////////////////////
	// DELETE ACTIVITY BY ID
	////////////////////////////////////////////////////////////////////////////
	public function delete($id, $type, $user=NULL) {
		$user = $this->uid($user);
		if (empty($user)) return false;

		return $this->pudl->delete('activity', [
			'user_id'			=> $user,
			'object_id'			=> $id,
			'object_type_id'	=> $this->af->type($type),
		]);
	}




	////////////////////////////////////////////////////////////////////////////
	// DELETE ACTIVITY BY FILE HASH
	////////////////////////////////////////////////////////////////////////////
	public function deleteFile($hash, $type, $user=NULL) {
		$user = $this->uid($user);
		if (empty($user)) return false;

		return $this->pudl->delete('activity', [
			'user_id'			=> $user,
			'file_hash'			=> $hash,
			'object_type_id'	=> $this->af->type($type),
		]);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE USER ID
	////////////////////////////////////////////////////////////////////////////
	protected function uid($user=NULL) {
		if (!empty($user)) {
			if (tbx_array($user)) return (int) $user['user_id'];
			return (int) $user;
		}
		return (int) $this->user->id();
	}




	////////////////////////////////////////////////////////////////////////////
	// LOCAL MEMBER VARIABLES
	////////////////////////////////////////////////////////////////////////////
	protected	$af		= NULL;
	protected	$user	= NULL;
	protected	$pudl	= NULL;

}
