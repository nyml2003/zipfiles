import React, { useEffect } from 'react';
import FileList from './FileList';
import BackupOption from './BackupOption';
import { TabPaneProps, Tabs, TabsProps } from 'antd';
import Explorer from '@/components/Explorer';
import { useSelector } from 'react-redux';
import { RootState } from '@/stores/store';

const NewCommit: React.FC = () => {
  const [activeKey, setActiveKey] = React.useState('1');
  const addExplorer = () => {
    setPanes((prev: TabsProps['items']) => {
      if (!prev) {
        return [
          {
            label: '新建备份列表',
            key: '1',
            children: (
              <>
                <FileList addExplorer={addExplorer} />
                <BackupOption />
              </>
            ),
            closable: false,
          },
          {
            label: '浏览文件',
            key: '2',
            children: <Explorer />,
          },
        ];
      }
      const newKey = String(Number(prev[prev.length - 1].key) + 1);
      setActiveKey(newKey);
      return [
        ...prev,
        {
          label: '浏览文件',
          key: newKey,
          children: <Explorer />,
        },
      ];
    });
  };
  const [panes, setPanes] = React.useState<TabsProps['items']>([
    {
      label: '新建备份列表',
      key: '1',
      children: (
        <>
          <FileList addExplorer={addExplorer} />
          <BackupOption />
        </>
      ),
      closable: false,
      className: 'astyle',
    },
  ]);

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
    <div className='w-full h-full flex flex-col bg-white'>
      <Tabs
        type='editable-card'
        activeKey={activeKey}
        onChange={setActiveKey}
        onEdit={handleEdit}
        hideAdd
        className='flex-1'
        items={panes}></Tabs>
    </div>
  );
};

export default NewCommit;
