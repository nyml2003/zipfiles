import { Button, Form, Input, Space } from "antd";
import { useContext, useState } from "react";
import { Context } from "./store/context";
import React from "react";
import { RestoreRequest } from "@/apis/Restore";
import PathChecker from "./PathChecker";

const RestoreForm = () => {
  const [form] = Form.useForm();
  const [password, setPassword] = useState("");
  const { state } = useContext(Context);
  const initialValues: RestoreRequest = {
    commitId: state.commitId,
    messageId: "",
    path: "",
  };

  const onFinish = (values: RestoreRequest) => {
    console.log("Received values:", values);
  };

  const onReset = () => {
    form.resetFields();
  };
  return (
    <Form
      className='fade-in-down p-4 max-w-3xl'
      labelCol={{ span: 4 }}
      wrapperCol={{ span: 14 }}
      form={form}
      layout='horizontal'
      variant='outlined'
      initialValues={initialValues}
      scrollToFirstError
      onFinish={onFinish}>
      <Form.Item label='提交ID' name='commitId' labelCol={{ span: 4 }} wrapperCol={{ span: 14 }}>
        <Input value={state.commitId} disabled />
      </Form.Item>
      {state.isEncrypt && (
        <Form.Item label='密码' name='password' rules={[{ required: true, message: "请输入密码" }]}>
          <Input.Password placeholder='请输入密码' autoComplete='current-password' />
        </Form.Item>
      )}
      <Form.Item name='path' label='路径' rules={[{ required: true, message: "请选择路径!" }]}>
        <PathChecker form={form} />
      </Form.Item>
      <Form.Item wrapperCol={{ offset: 4, span: 14 }}>
        <Space>
          <Button type='primary' htmlType='submit'>
            Submit
          </Button>
          <Button htmlType='button' onClick={onReset}>
            Reset
          </Button>
        </Space>
      </Form.Item>
    </Form>
  );
};

export default RestoreForm;
