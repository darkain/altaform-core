<?php


namespace af;



////////////////////////////////////////////////////////////////////////////////
// HANDLES PERMISSION CHECKING FOR A USER
////////////////////////////////////////////////////////////////////////////////
trait permission {




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF THE USER HAS THE GIVEN PERMISSION LEVEL (GLOBAL)
	////////////////////////////////////////////////////////////////////////////
	public function hasPermission($permission) {
		if (empty($this->permission)) $this->permissions();

		if (!tbx_array($permission)) {
			$permission = explode(',', $permission);
		}

		foreach ($permission as $perm) {
			$perm = trim($perm);
			if (!empty($this->permission[$perm])) return true;
		}

		return false;
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF THE USER HAS 'ADMIN' PERMISSION LEVEL
	////////////////////////////////////////////////////////////////////////////
	public function isAdmin() {
		return $this->hasPermission('admin');
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF THE USER HAS 'STAFF' OR 'ADMIN' PERMISSION LEVELS
	////////////////////////////////////////////////////////////////////////////
	public function isStaff() {
		return $this->hasPermission(['staff','admin']);
	}




	////////////////////////////////////////////////////////////////////////////
	// REQUIRE THE GIVEN PERMISSION LEVEL, OR EXIT TO HTTP ERROR STATUS SCREEN
	////////////////////////////////////////////////////////////////////////////
	public function requirePermission($permission, $code=401) {
		\af\affirm($code,
			$this->hasPermission($permission),
			'This page requires the following permission level: '
			. (tbx_array($permission) ? implode(', ', $permission) : $permission)
		);

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// REQUIRE THE 'ADMIN' PERMISSION LEVEL
	////////////////////////////////////////////////////////////////////////////
	public function requireAdmin($code=401) {
		return $this->requirePermission('admin', $code);
	}




	////////////////////////////////////////////////////////////////////////////
	// REQUIRE THE 'STAFF' OR 'ADMIN' PERMISSION LEVELS
	////////////////////////////////////////////////////////////////////////////
	public function requireStaff($code=401) {
		return $this->requirePermission(['staff','admin'], $code);
	}




	////////////////////////////////////////////////////////////////////////////
	// PROCESS THE USER PERMISSIONS LIST DATA
	////////////////////////////////////////////////////////////////////////////
	public function permissions() {
		global $af;

		//DEFAULT USER ACCESS RIGHTS
		$this->permission = $af->config->permission;

		if (empty($this->user_permission)) {
			$this->user_permission = 'guest';
		}

		$permissions = explode(',', $this->user_permission);
		foreach ($permissions as $val) {
			$val = trim($val);
			$this->permission[$val] = 1;
		}

		if (!empty($this->user_adfree)) {
			if ($this->user_adfree > $this->pudl()->time()) {
				$this->permission['adfree'] = 1;
			}
		}

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF THE USER HAS THE GIVEN ACCESS RIGHTS (OBJECT)
	////////////////////////////////////////////////////////////////////////////
	public function hasAccess($access, $id=false) {
		if (empty($this->access)) $this->access();

		if (!tbx_array($access)) {
			$access = explode(',', $access);
			foreach ($access as &$item) $item = trim($item);
		}

		foreach ($access as $item) {
			if (!empty($this->access[$item])) {
				return $this->access[$item];
			}
		}

		if ($id !== false) {
			if ($id instanceof pudlOrm) {
				$id = $id->id();
			} else if (tbx_array($id)) {
				$id = $id['object_id'];
			}

			$type = altaform::type(reset($access), $this->pudl());

			$row = $this->pudl()->row('object_access', [
				'object_id'			=> $id,
				'object_type_id'	=> $type,
				'user_id'			=> $this->id(),
			]);

			if (!empty($row)) return $row['object_access'];
		}

		return false;
	}




	////////////////////////////////////////////////////////////////////////////
	// REQUIRE THE USER HAS THE GIVEN ACCESS RIGHTS, OR EXIT TO HTTP ERROR
	////////////////////////////////////////////////////////////////////////////
	public function requireAccess($access, $id=false, $code=401) {
		\af\affirm($code,
			$this->hasAccess($access, $id),
			'This page requires the following access rights: '
			. (tbx_array($access) ? implode(', ', $access) : $access)
		);
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// PROCESS THE USER ACCESS RIGHTS DATA
	////////////////////////////////////////////////////////////////////////////
	public function access() {
		if (isset($this->access)) return $this;

		$this->access	= [];
		$access			= $this->pudl()->rowsId('user_access', $this);

		foreach ($access as $item) {
			$this->access[ $item['user_access'] ] = $item['object_access'];
		}

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER HAS EITHER OBJECT ACCESS OR GLOBAL PERMISSIONS
	////////////////////////////////////////////////////////////////////////////
	public function hasAccessPermission($access, $permission, $id=false) {
		return $this->hasAccess($access, $id)
			|| $this->hasPermission($permission);
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER HAS EITHER OBJECT ACCESS OR GLOBAL ADMIN PERMISSIONS
	////////////////////////////////////////////////////////////////////////////
	public function hasAccessAdmin($access, $id=false) {
		return $this->hasAccessPermission($access, 'admin', $id);
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF USER HAS EITHER OBJECT ACCESS OR GLOBAL STAFF/ADMIN PERMISSIONS
	////////////////////////////////////////////////////////////////////////////
	public function hasAccessStaff($access, $id=false) {
		return $this->hasAccessPermission($access, ['staff','admin'], $id);
	}




	////////////////////////////////////////////////////////////////////////////
	// REQUIRE OBJECT ACCESS OR GLOBAL PERMISSIONS
	////////////////////////////////////////////////////////////////////////////
	public function requireAccessPermission($access, $permission, $id=false, $code=401) {
		\af\affirm($code,
			$this->hasAccessPermission($access, $permission, $id),
			'This page requires the following permission level: '
			. (tbx_array($access)		? implode(', ', $access)		: $access)
			. ', '
			. (tbx_array($permission)	? implode(', ', $permission)	: $permission)
		);
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// REQUIRE OBJECT ACCESS OR GLOBAL ADMIN PERMISSIONS
	////////////////////////////////////////////////////////////////////////////
	public function requireAccessAdmin($access, $id=false) {
		return $this->requireAccessPermission($access, 'admin', $id);
	}




	////////////////////////////////////////////////////////////////////////////
	// REQUIRE OBJECT ACCESS OR GLOBAL STAFF/ADMIN PERMISSIONS
	////////////////////////////////////////////////////////////////////////////
	public function requireAccessStaff($access, $id=false) {
		return $this->requireAccessPermission($access, ['staff','admin'], $id);
	}




	////////////////////////////////////////////////////////////////////////////
	// MEMBER VARIABLES
	////////////////////////////////////////////////////////////////////////////
//	protected	$permission		= [];
	protected	$permissions	= [];

}
