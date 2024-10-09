import { Splitter } from 'antd';
import React from 'react';
import MetaDataCard from './MetaDataCard';
import TreeMenu from './TreeMenu';
import styles from '../index.module.less';
import TableView from './TableView';

const TreeView = () => {
  return (
    <Splitter className={`flex-1 overflow-hidden ${styles['fade-in-down']}`}>
      <Splitter.Panel
        defaultSize='20%'
        min='10%'
        max='70%'>
        <TreeMenu />
      </Splitter.Panel>
      <Splitter.Panel>
        <TableView></TableView>
      </Splitter.Panel>
    </Splitter>
  );
};

export default TreeView;
