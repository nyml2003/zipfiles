import { removeNotification } from "@/stores/NotificationReducer";
import { RootState } from "@/stores/store";
import { useDispatch, useSelector } from "react-redux";
import {
  NotificationUnion,
  PlainText as TypePlainText,
  CommitPush as TypeCommitPush,
  CommitRestore as TypeCommitRestore,
} from "@/components/NotificationList/types";
import React from "react";
import NoMoreData from "../NoMoreData";
import PlainText from "./PlainText";
import CommitPush from "./CommitPush";
import { Button } from "antd";
import { CloseOutlined } from "@ant-design/icons";
import CommitRestore from "./CommitRestore";

const NotificationList = () => {
  const notifications = useSelector((state: RootState) => state.notification.notifications);
  const dispatch = useDispatch();

  const closeNotification = (id: string) => {
    dispatch(removeNotification(id));
  };

  const renderItem = (notification: NotificationUnion) => {
    switch (notification.type) {
      case "plainText": {
        return <PlainText {...(notification as TypePlainText)} />;
      }
      case "commitPush": {
        return <CommitPush {...(notification as TypeCommitPush)} />;
      }
      case "commitRestore": {
        return <CommitRestore {...(notification as TypeCommitRestore)} />;
      }
      default:
        return null;
    }
  };

  return (
    <div className='w-25vw fade-in-left flex-col align-start overflow-auto hidden'>
      <div className='text-lg font-bold p-4'>消息列表</div>
      {notifications.length > 0 ? (
        notifications.map(notification => (
          <div
            key={notification.id}
            className='m-2 px-6 py-5 bg-white shadow-md rounded-lg relative'>
            {renderItem(notification)}
            <Button
              type='text'
              onClick={() => closeNotification(notification.id)}
              className='absolute top-2 right-2'
              size='small'>
              <CloseOutlined />
            </Button>
          </div>
        ))
      ) : (
        <NoMoreData />
      )}
    </div>
  );
};

export default NotificationList;
