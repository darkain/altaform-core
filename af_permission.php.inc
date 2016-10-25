<?php


trait af_permission {


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



	public function isAdmin() { return $this->hasPermission('admin'); }
	public function isStaff() { return $this->hasPermission(['staff','admin']); }



	public function requirePermission($permission) {
		assert401(
			$this->hasPermission($permission),
			'This page requires the following permission level: '
			. (tbx_array($permission) ? implode(', ', $permission) : $permission)
		);
		return $this;
	}




	public function requireAdmin() { return $this->requirePermission('admin'); }
	public function requireStaff() { return $this->requirePermission(['staff','admin']); }




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
			if ($this->user_adfree > $af->time()) {
				$this->permission['adfree'] = 1;
			}
		}

		return $this;
	}




	public function hasAccess($access, $id=false) {
		global $af, $db;

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
			$row = $db->row('pudl_object_access', [
				'object_id'			=> tbx_array($id) ? $id['object_id'] : $id,
				'object_type_id'	=> $af->type(reset($access)),
				'user_id'			=> $this->id(),
			]);

			if (!empty($row)) return $row['object_access'];
		}

		return false;
	}




	public function requireAccess($access, $id=false) {
		assert401(
			$this->hasAccess($access, $id),
			'This page requires the following permission level: '
			. (tbx_array($access) ? implode(', ', $access) : $access)
		);
		return $this;
	}




	public function access() {
		global $db;

		if (isset($this->access)) return $this;

		$this->access	= [];
		$access			= $db->rowsId('pudl_user_access', $this);

		foreach ($access as $item) {
			$this->access[ $item['user_access'] ] = $item['object_access'];
		}

		return $this;
	}




	public function hasAccessPermission($access, $permission, $id=false) {
		return $this->hasAccess($access, $id)
			|| $this->hasPermission($permission);
	}




	public function hasAccessAdmin($access, $id=false) {
		return $this->hasAccessPermission($access, 'admin', $id);
	}




	public function hasAccessStaff($access, $id=false) {
		return $this->hasAccessPermission($access, ['staff','admin'], $id);
	}




	public function requireAccessPermission($access, $permission, $id=false) {
		assert401(
			$this->hasAccessPermission($access, $permission, $id),
			'This page requires the following permission level: '
			. (tbx_array($access)		? implode(', ', $access)		: $access)
			. ', '
			. (tbx_array($permission)	? implode(', ', $permission)	: $permission)
		);
		return $this;
	}




	public function requireAccessAdmin($access, $id=false) {
		return $this->requireAccessPermission($access, 'admin', $id);
	}




	public function requireAccessStaff($access, $id=false) {
		return $this->requireAccessPermission($access, ['staff','admin'], $id);
	}

}
