import { Button, Form, Input, Space } from "antd";
import React, { useCallback, useContext, useEffect } from "react";
import { Context } from "./store";
import PathChecker from "./PathChecker";
import { useDispatch } from "react-redux";
import { addNotification } from "@/stores/NotificationReducer";
import { CommitRestore } from "@/components/NotificationList/types";

type FormDataType = {
  commitId: string;
  path: string;
  password?: string;
};

const RestoreForm = () => {
  const [form] = Form.useForm();
  const { state } = useContext(Context);
  const dispatch = useDispatch();
  const initialValues: FormDataType = {
    commitId: state.commitId,
    path: "",
    password: "",
  };

  const onFinish = (values: FormDataType) => {
    dispatch(
      addNotification({
        type: "commitRestore",
        progress: 0,
        ...values,
      } as Omit<CommitRestore, "id">),
    );
  };

  const onReset = () => {
    form.resetFields();
  };

  const selectedPath = useCallback(() => {
    if (state.file !== "") {
      return state.file;
    }
    if (state.path !== "") {
      return state.path;
    }
    return "";
  }, [state.path, state.file]);

  useEffect(() => {
    form.setFieldsValue({ path: selectedPath() });
  }, [state.path, state.file]);

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
        <Input value={state.commitId} disabled autoComplete='off' />
      </Form.Item>
      {state.isEncrypt && (
        <Form.Item label='密码' name='password' rules={[{ required: true, message: "请输入密码" }]}>
          <Input.Password placeholder='请输入密码' autoComplete='current-password' />
        </Form.Item>
      )}
      <Form.Item name='path' label='路径' rules={[{ required: true, message: "请选择路径!" }]}>
        <div>
          <Input value={selectedPath()} disabled />
          <PathChecker />
        </div>
      </Form.Item>
      <Form.Item wrapperCol={{ offset: 4, span: 14 }}>
        <Space>
          <Button type='primary' htmlType='submit'>
            提交
          </Button>
          <Button htmlType='button' onClick={onReset}>
            重置
          </Button>
        </Space>
      </Form.Item>
    </Form>
  );
};

export default RestoreForm;
