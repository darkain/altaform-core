<?php



require_once('orm.php');



class           afo_photo
    extends     afo_orm {
    use         afo_template;




    ////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTOR
    ////////////////////////////////////////////////////////////////////////////
    public function __construct($item=false, $fetch=false) {
        parent::__construct($item, $fetch);
        $this->id = bin2hex($this->file_hash);
    }




    ////////////////////////////////////////////////////////////////////////////
    // URL TO THIS OBJECT
	// TODO: UPDATE THIS WITH CUSTOM CONFIGURABLE BASE, LIKE IN GALLERY
    ////////////////////////////////////////////////////////////////////////////
    public function url() {
        global $afurl;
        return $afurl->user(
            $this,
            'gallery',
            $this->gallery_id,
            bin2hex($this->file_hash)
        );
    }




    ////////////////////////////////////////////////////////////////////////////
    // PROCESS PROMETHEUS STYLE FORMATTING
    // FIX GALLERY ID AND NAME
    ////////////////////////////////////////////////////////////////////////////
    public function prometheus($size=300) {
        if (empty($this->gallery_id)  &&  !empty($this->g_id)) {
            $this->gallery_id   = $this->g_id;
            $this->gallery_name = $this->g_name;
        }

        return parent::prometheus($size);
    }




    ////////////////////////////////////////////////////////////////////////////
    // LATE STATIC BINDING VARIABLES FROM PUDL ORM
    ////////////////////////////////////////////////////////////////////////////
    const   column      = 'file_hash';
    const   icon        = 'fl.file_hash';
    const   table       = 'pudl_file';
    const   prefix      = 'fl';
    const   hash        = true;

}
