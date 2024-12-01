import React from "react";
import CommitTable from "./CommitTable";
import { Tabs, TabsProps } from "antd";
import Explorer from "./Explorer";
import { createProvider as createExplorerProvider } from "./Explorer/store";
import { createProvider as createRestoreProvider } from "./Restore/store";
import { v4 as uuidv4 } from "uuid";
import RestoreForm from "./Restore/RestoreForm";
import RecycleBin from "./RecycleBin";

const ExplorerPane = (uuid: string, key: string) => ({
  label: "浏览文件",
  key,
  children: createExplorerProvider(<Explorer />, { commitId: uuid }),
  className: "grow-item",
});

const RestorePane = (uuid: string, key: string, isEncrypt: boolean) => ({
  label: "恢复文件",
  key,
  children: createRestoreProvider(<RestoreForm />, { commitId: uuid, isEncrypt }),
  className: "grow-item",
});

const CommitPage: React.FC = () => {
  const [activeKey, setActiveKey] = React.useState("commitList");
  const openExplorer = (uuid: string) => {
    setPanes((prev: TabsProps["items"]) => {
      const newKey = uuidv4();
      if (!prev) {
        return [...InitialPanes, ExplorerPane(uuid, newKey)];
      }
      setActiveKey(newKey);
      return [...prev, ExplorerPane(uuid, newKey)];
    });
  };

  const openRestore = (uuid: string, isEncrypt: boolean) => {
    setPanes((prev: TabsProps["items"]) => {
      const newKey = uuidv4();
      if (!prev) {
        return [...InitialPanes, RestorePane(uuid, newKey, isEncrypt)];
      }
      setActiveKey(newKey);
      return [...prev, RestorePane(uuid, newKey, isEncrypt)];
    });
  };
  const InitialPanes = [
    {
      label: "提交列表",
      key: "commitList",
      children: <CommitTable openExplorer={openExplorer} openRestore={openRestore} />,
      closable: false,
      className: "grow-item",
    },
    {
      label: "回收站",
      key: "recycle",
      children: <RecycleBin />,
      closable: false,
      className: "grow-item",
    },
  ];

  const [panes, setPanes] = React.useState<TabsProps["items"]>(InitialPanes);

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
        destroyInactiveTabPane={true}
        className='split-container-col grow-item'
        items={panes}></Tabs>
    </div>
  );
};

export default CommitPage;
