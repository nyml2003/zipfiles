import React from "react";
import FileList from "./FileList";
import BackupOption from "./BackupOption";
import { Tabs, TabsProps } from "antd";
import Explorer from "./Explorer";

const NewCommit: React.FC = () => {
  const [activeKey, setActiveKey] = React.useState("1");
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
            children: <Explorer />,
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
          children: <Explorer />,
          className: "grow-item",
        },
      ];
    });
  };
  const [panes, setPanes] = React.useState<TabsProps["items"]>([
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

export default NewCommit;
