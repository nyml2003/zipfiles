import React, { FC, useState } from "react";
import FileList from "./FileList";
import BackupOption from "./BackupOption";
import { Tabs, TabsProps } from "antd";
import Explorer from "./Explorer";

const NewCommit: FC = () => {
  const [activeKey, setActiveKey] = useState("1");
  const createCloseExplorer = (key: string) => {
    return () => {
      setActiveKey("1");
      setPanes((prev: TabsProps["items"]) => {
        if (!prev) {
          return [];
        }
        return prev.filter(pane => pane.key !== key);
      });
    };
  };
  const addExplorer = () => {
    setPanes((prev: TabsProps["items"]) => {
      if (!prev) {
        return [
          {
            label: "新建备份列表",
            key: "1",
            children: (
              <>
                <FileList addExplorer={addExplorer} />
                <BackupOption />
              </>
            ),
            closable: false,
            className: "grow-item",
          },
          {
            label: "浏览文件",
            key: "2",
            children: <Explorer closeExplorer={createCloseExplorer("2")} />,
            className: "grow-item",
          },
        ];
      }
      const newKey = String(Number(prev[prev.length - 1].key) + 1);
      setActiveKey(newKey);
      return [
        ...prev,
        {
          label: "浏览文件",
          key: newKey,
          children: <Explorer closeExplorer={createCloseExplorer(newKey)} />,
          className: "grow-item",
        },
      ];
    });
  };
  const [panes, setPanes] = useState<TabsProps["items"]>([
    {
      label: "创建备份",
      key: "1",
      children: (
        <>
          <FileList addExplorer={addExplorer} />
          <BackupOption />
        </>
      ),
      closable: false,
      className: "grow-item",
    },
  ]);

  const handleEdit = (
    e: React.MouseEvent | React.KeyboardEvent | string,
    action: "add" | "remove",
  ) => {
    if (!panes) {
      return;
    }
    if (action === "remove") {
      const newPanes = panes.filter(pane => pane.key !== e);
      setActiveKey(newPanes.length > 0 ? newPanes[newPanes.length - 1].key : "");
      setPanes(newPanes);
    }
  };

  return (
    <div className=' grow-item split-container-row'>
      <Tabs
        type='editable-card'
        activeKey={activeKey}
        onChange={setActiveKey}
        onEdit={handleEdit}
        hideAdd
        className='split-container-col grow-item bg-white p-2'
        items={panes}></Tabs>
    </div>
  );
};

export default NewCommit;
