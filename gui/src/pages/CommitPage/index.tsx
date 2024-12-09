import React from "react";
import CommitTable from "./CommitTable";
import { Tabs, TabsProps } from "antd";
import Explorer from "./Explorer";
import { createProvider as createExplorerProvider } from "./Explorer/store";
import { createProvider as createRestoreProvider } from "@/components/PathChecker/store";
import { v4 as uuidv4 } from "uuid";
import RestoreForm from "./Restore/RestoreForm";
import RecycleBin from "./RecycleBin";

const ExplorerPane = (uuid: string, key: string) => ({
  label: <div>浏览文件</div>,
  key,
  children: createExplorerProvider(<Explorer commitId={uuid} />),
  className: "grow-item"
});

const RestorePane = (uuid: string, key: string, isEncrypt: boolean) => ({
  label: <div>恢复备份</div>,
  key,
  children: createRestoreProvider(<RestoreForm commitId={uuid} isEncrypt={isEncrypt} />),
  className: "grow-item"
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
      label: <div className='text-center'>回收站</div>,
      key: "recycle",
      children: <RecycleBin />,
      closable: false,
      className: "grow-item"
    },
    {
      label: <div className='text-center'>备份列表</div>,
      key: "commitList",
      children: <CommitTable openExplorer={openExplorer} openRestore={openRestore} />,
      closable: false,
      className: "grow-item"
    }
  ];

  const [panes, setPanes] = React.useState<TabsProps["items"]>(InitialPanes);

  const handleEdit = (
    e: React.MouseEvent | React.KeyboardEvent | string,
    action: "add" | "remove"
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
    <Tabs
      type='editable-card'
      activeKey={activeKey}
      onChange={setActiveKey}
      onEdit={handleEdit}
      hideAdd
      destroyInactiveTabPane={true}
      className=' bg-white p-2'
      style={{ height: "calc(100vh - 56px)" }}
      items={panes}></Tabs>
  );
};

export default CommitPage;
