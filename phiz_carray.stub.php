<?php
/** @generate-function-entries */
class CArray implements  IteratorAggregate, ArrayAccess, Countable {
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

    /** @return bool */
    public function setSize(int $size) {}

    /** @return bool */
    public function isSignedType() {}

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


    public function getIterator(): Iterator {}
}