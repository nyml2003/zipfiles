import React from 'react';
import CommitTable from './CommitTable';
import { Tabs, TabsProps } from 'antd';
import Explorer from './Explorer';

const ExplorerPane = (key?: string) => ({
  label: '浏览文件',
  key: key || '2',
  children: <Explorer />,
  className: 'grow-item',
});

const CommitPage: React.FC = () => {
  const [activeKey, setActiveKey] = React.useState('1');
  const openExplorer = () => {
    setPanes((prev: TabsProps['items']) => {
      if (!prev) {
        return [InitialPane, ExplorerPane()];
      }
      const newKey = String(Number(prev[prev.length - 1].key) + 1);
      setActiveKey(newKey);
      return [...prev, ExplorerPane(newKey)];
    });
  };
  const InitialPane = {
    label: '提交列表',
    key: '1',
    children: <CommitTable openExplorer={openExplorer} />,
    closable: false,
    className: 'grow-item',
  };

  const [panes, setPanes] = React.useState<TabsProps['items']>([InitialPane]);

  const handleEdit = (
    e: React.MouseEvent | React.KeyboardEvent | string,
    action: 'add' | 'remove',
  ) => {
    if (!panes) {
      return;
    }
    if (action === 'remove') {
      const newPanes = panes.filter(pane => pane.key !== e);
      setActiveKey(newPanes.length > 0 ? newPanes[newPanes.length - 1].key : '');
      setPanes(newPanes);
    }
  };

  return (
    <div className='bg-white grow-item split-container-row p-4'>
      <Tabs
        type='editable-card'
        activeKey={activeKey}
        onChange={setActiveKey}
        onEdit={handleEdit}
        hideAdd
        className='split-container-col grow-item'
        items={panes}></Tabs>
    </div>
  );
};

export default CommitPage;
