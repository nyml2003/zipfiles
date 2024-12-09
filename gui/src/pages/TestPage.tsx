import { ApiEnum } from "@/apis";
import {
  MockManyNotificationsRequest,
  MockManyNotificationsResponse
} from "@/apis/MockManyNotifications";
import useApi from "@useApi";
import { Button, message } from "antd";
import React from "react";

const TestPage = () => {
  const api = useApi();
  const tryMockManyNotifications = () => {
    api
      .request<MockManyNotificationsRequest, MockManyNotificationsResponse>(
        ApiEnum.MOCK_MANY_NOTIFICATIONS,
        {}
      )
      .then(res => {
        message
          .success({
            content: JSON.stringify(res),
            duration: 2
          })
          .then();
      });
  };
  return (
    <div>
      <h1>Test Page</h1>
      <div>
        <Button onClick={tryMockManyNotifications} type='primary' className='mt-4'>
          测试按钮
        </Button>
      </div>
    </div>
  );
};

export default TestPage;
