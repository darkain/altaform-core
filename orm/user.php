<?php


require_once('orm.php');
require_once('filter.php');




class			afo_user
	extends		afo_orm {
	use			afo_template;




	////////////////////////////////////////////////////////////////////////////
	//URL TO THIS OBJECT
	////////////////////////////////////////////////////////////////////////////
	public function url() {
		global $afurl;
		return $afurl->user($this);
	}




	////////////////////////////////////////////////////////////////////////////
	//OVERWRITE THE PUDL PARAMETERS FOR PULLING A COLLECTION
	////////////////////////////////////////////////////////////////////////////
	protected static function schema() {
		return array_merge_recursive(parent::schema(), [
			'column'	=> ['uo.user_tagline'],

			'table'		=> ['uo' => 'pudl_user_profile'],

			'clause'	=> [
				af_filter_banned(static::prefix),
				'uo.user_id' => pudl::column([static::prefix,static::column]),
			],
		]);
	}




	////////////////////////////////////////////////////////////////////////////
	//LATE STATIC BINDING VARIABLES FROM PUDL ORM
	////////////////////////////////////////////////////////////////////////////
	const	name		= 'user_tagline';
	const	column		= 'user_id';
	const	icon		= 'us.user_icon';
	const	table		= 'pudl_user';
	const	prefix		= 'us';

}
