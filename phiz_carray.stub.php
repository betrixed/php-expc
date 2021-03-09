<?php
/** @generate-function-entries */
class CArray implements  ArrayAccess, Countable {
	public function __construct(int $size, int $cptype) {}

    /** @return int */
    public function count() {}

    /** @return array */
    public function toArray() {}

	    /** @return int */
    public function getSize() {}

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

}