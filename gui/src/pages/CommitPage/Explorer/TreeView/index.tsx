import { Splitter } from 'antd';
import React from 'react';
import TreeMenu from './TreeMenu';
import TableView from './TableView';

const TreeView = () => {
  return (
    <Splitter className='fade-in-down split-container-row grow-item'>
      <Splitter.Panel
        defaultSize='20%'
        min='10%'
        max='70%'
        className='split-container-row grow-item'>
        <TreeMenu />
      </Splitter.Panel>
      <Splitter.Panel className='split-container-row grow-item'>
        <TableView/>
      </Splitter.Panel>
    </Splitter>
  );
};

export default TreeView;
