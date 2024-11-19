import React, { useRef, useState } from "react";
import { HomeOutlined, SettingOutlined, UploadOutlined } from "@ant-design/icons";
import type { TourStepProps } from "antd";
import { MenuInfo } from "rc-menu/lib/interface";
import { Layout, Menu, Tour } from "antd";
import { Outlet, useNavigate } from "react-router-dom";
import FootContent from "./FootContent";
import NotificationList from "@/components/NotificationList";
import { CSSTransition } from "react-transition-group";
import { useSelector } from "react-redux";
import { RootState } from "@/stores/store";

const { Content, Sider } = Layout;

const App: React.FC = () => {
  const navigate = useNavigate();
  const [open, setOpen] = useState(false);
  const openTour = () => setOpen(true);
  const notificationVisible = useSelector((state: RootState) => state.notification.open);
  const headerItems = [
    {
      key: "index",
      label: <div ref={el => (headerItemsRef.current[0] = el)}>首页</div>,
      icon: React.createElement(HomeOutlined),
    },
    {
      key: "new_commit",
      label: <div ref={el => (headerItemsRef.current[1] = el)}>新建提交</div>,
      icon: React.createElement(UploadOutlined),
    },
    {
      key: "commit_history",
      label: <div ref={el => (headerItemsRef.current[2] = el)}>提交历史</div>,
      icon: React.createElement(UploadOutlined),
    },
    {
      key: "setting",
      label: <div ref={el => (headerItemsRef.current[3] = el)}>设置</div>,
      icon: React.createElement(UploadOutlined),
    },
    {
      key: "test",
      label: "测试",
      icon: React.createElement(SettingOutlined),
    },
  ];

  const handleClick = (info: MenuInfo) => {
    if (info.key === "github") {
      navigate("/index");
      return;
    }
    navigate(info.key);
  };

  const headerItemsRef = useRef(
    Array.from({ length: headerItems.length }).map(() => null as HTMLDivElement | null),
  );

  const steps: TourStepProps[] = [
    {
      title: "欢迎使用",
      description: "这是您的应用首页。",
      placement: "right",
      target: headerItemsRef.current[0]!,
    },
    {
      title: "新建提交",
      description: "点击这里可以快速创建新的提交，方便您的工作流程。",
      placement: "right",
      target: headerItemsRef.current[1]!,
    },
    {
      title: "提交历史",
      description: "在这里查看您所有的提交历史，便于管理和追踪。",
      placement: "right",
      target: headerItemsRef.current[2]!,
    },
    {
      title: "设置",
      description: "在设置中，您可以自定义应用的各种选项。",
      placement: "right",
      target: headerItemsRef.current[3]!,
    },
  ];

  return (
    <>
      <Layout hasSider className='select-none flex flex-row h-screen'>
        <Sider breakpoint='lg' className=' bg-white'>
          <Menu
            defaultSelectedKeys={["1"]}
            defaultOpenKeys={["sub1"]}
            onClick={handleClick}
            items={headerItems}
            className='bg-white'
            style={{
              borderInlineEnd: "none",
            }}></Menu>
        </Sider>
        <Layout className='grow-item split-container-row overflow-x-hidden'>
          <div className='grow-item split-container-col'>
            <Content className='grow-item'>
              <Outlet context={{ openTour }} />
            </Content>
            <FootContent />
          </div>
          <CSSTransition in={notificationVisible} timeout={500} classNames='fade' unmountOnExit>
            <NotificationList />
          </CSSTransition>
        </Layout>
      </Layout>
      <Tour open={open} onClose={() => setOpen(false)} steps={steps} />
    </>
  );
};

export default App;
