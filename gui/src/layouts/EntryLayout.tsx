import React from "react";
import { HistoryOutlined, HomeOutlined, PlusOutlined, SettingOutlined } from "@ant-design/icons";
import { Layout, Menu } from "antd";
import { Outlet, useNavigate } from "react-router-dom";
import FootContent from "./FootContent";
import NotificationList from "@/components/NotificationList";
import { CSSTransition } from "react-transition-group";
import { useSelector } from "react-redux";
import { RootState } from "@/stores/store";

const { Content, Sider } = Layout;

const App: React.FC = () => {
  const notificationVisible = useSelector((state: RootState) => state.notification.open);
  const headerItems = [
    {
      key: "index",
      label: "首页",
      icon: React.createElement(HomeOutlined),
    },
    {
      key: "new_commit",
      label: "新建",
      icon: React.createElement(PlusOutlined),
    },
    {
      key: "commit_history",
      label: "历史",
      icon: React.createElement(HistoryOutlined),
    },
    {
      key: "setting",
      label: "设置",
      icon: React.createElement(SettingOutlined),
    },
  ];

  // const steps: TourStepProps[] = [
  //   {
  //     title: "欢迎使用",
  //     description: "这是您的应用首页。",
  //     placement: "right",
  //     target: headerItemsRef.current[0]!,
  //   },
  //   {
  //     title: "新建提交",
  //     description: "点击这里可以快速创建新的提交，方便您的工作流程。",
  //     placement: "right",
  //     target: headerItemsRef.current[1]!,
  //   },
  //   {
  //     title: "提交历史",
  //     description: "在这里查看您所有的提交历史，便于管理和追踪。",
  //     placement: "right",
  //     target: headerItemsRef.current[2]!,
  //   },
  //   {
  //     title: "设置",
  //     description: "在设置中，您可以自定义应用的各种选项。",
  //     placement: "right",
  //     target: headerItemsRef.current[3]!,
  //   },
  // ];
  const navigate = useNavigate();

  return (
    <>
      <Layout hasSider className='select-none flex flex-row h-screen'>
        <Sider breakpoint='lg' className='bg-white'>
          <Menu
            defaultSelectedKeys={["index"]}
            defaultOpenKeys={["index"]}
            onClick={info => navigate(`/${info.key}`)}
            items={headerItems}
            className='bg-white'
            style={{
              borderInlineEnd: "none",
            }}></Menu>
        </Sider>
        <Layout className='grow-item split-container-row overflow-x-hidden'>
          <div className='grow-item split-container-col'>
            <Content className='grow-item'>
              <Outlet />
            </Content>
            <FootContent />
          </div>
          <CSSTransition
            in={notificationVisible}
            timeout={500}
            classNames='fade'
            unmountOnExit={false}>
            <NotificationList />
          </CSSTransition>
        </Layout>
      </Layout>
    </>
  );
};

export default App;
