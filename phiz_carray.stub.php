<?php
/** @generate-function-entries */
class CArray implements  ArrayAccess, Countable, Iterator {
	public function __construct(int $size, int $cptype) {}

    /** @return int */
    public function count() {}

    /** @return array */
    public function toArray() {}

    /** @return int */
    public  function getType() {}


    /** @return string */
    public function getTypeName() {}
    
    /** @return int */
    public function getTypeSize() {}

	    /** @return int */
    
    public function getSize() {}

    /** @return mixed 
     *  @param int $index
    */
    public function getValue(int $index) {}


    /**
     * @param int $index
     * @return void
     */
    public function setValue(int $index, mixed $value) {}

    /** @return bool */
    public function setSize(int $size) {}

    /**
     * @param int $index
     * @return bool
     */

	public function offsetExists($index) {}

    /**
     * @param int $index
     * @return mixed
     */
    public function offsetGet($index) {}

    /**
     * @param int $index
     * @return void
     */
    public function offsetSet($index, mixed $value) {}

    /**
     * @param int $index
     * @return void
     */
    public function offsetUnset($index) {}

    /**   @return mixed */
    public function current() {}

    /** @return scaler */
    public function key() {}

    /** @return void */
    public function next() {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

}