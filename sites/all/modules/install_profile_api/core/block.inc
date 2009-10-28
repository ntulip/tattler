<?php

/**
 * Add a new plain block provided by block module.
 */
function install_add_block($module, $delta, $theme, $status, $weight = 0, $region = '', $visibility = 0, $pages = '', $custom = 0, $throttle = 0, $title = '') {
  if ($status) {
    db_query("INSERT INTO {blocks} (module, delta, theme, status, weight, region, visibility, pages, custom, throttle, title) 
       VALUES ('%s', '%s', '%s', %d, %d, '%s', %d, '%s', %d, %d, '%s')", 
       $module, $delta, $theme, $status, $weight, $region, $visibility, $pages, $custom, $throttle, $title);
  }
  else {
    db_query("UPDATE {blocks} SET status = 0 WHERE module = '%s' AND delta = '%s' AND theme = '%s'", $module, $delta, $theme); 
  }
}

/**
 * Position an existing block inside a region of a theme.
 *
 * TIP: To identify the $module and $delta, go to an existing site and visit
 * the /admin/build/block page. Hover over the 'configure' links - the module
 * and delta are the last two parts of the target url.
 *
 * @param $theme
 *   A theme name, eg. 'garland'
 * @param $region
 *   Available region: usually one of 'header', 'footer', 'left', 'right', 'content'
 * @param $module
 *   The name of the module that provides the block
 * @param $delta
 *   The block id.
 * @param $weight
 *   Block order within the region.
 */
function install_set_block($module, $delta, $theme, $region, $weight = 0) {
  db_query("UPDATE {blocks} SET region = '%s', status = 1, weight = %d WHERE module = '%s' AND delta = '%s' AND theme = '%s'", $region, $weight, $module, $delta, $theme);
}

/**
* Disable a block within a theme.
*/
function install_disable_block($module, $delta, $theme) {
  db_query("UPDATE {blocks} SET region = '', status = 0 WHERE module = '%s' AND delta = '%s' AND theme = '%s'", $module, $delta, $theme);
}

/**
 * Creates a new block role.
 */
function install_add_block_role($module, $delta, $rid) {
  db_query("INSERT INTO {blocks_roles} (module,delta,rid) VALUES ('%s', '%s', %d)", $module, $delta, $rid);
}

/**
 * Create a custom block (box).
 *
 * @param $body
 *   The body of the custom block.
 * @param $description
 *   The description of the custom block (for admin/build/blocks).
 * @param $format
 *   The input format for the block's body.
 *
 * @return
 *   The new block's delta.
 *
 * @see block_add_block_form_submit()
 */
function install_create_custom_block($body, $description, $format = FILTER_FORMAT_DEFAULT) {
  db_query("INSERT INTO {boxes} (body, info, format) VALUES ('%s', '%s', %d)", $body, $description, $format);
  return db_last_insert_id('boxes', 'bid');
}

